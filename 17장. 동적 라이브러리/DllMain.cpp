/*
DLL파일은 코어파일이 항상 0x100000에 로드되는 것과 달리 선호되는 번지가 고정되어있지 않다.
진입점 기본 세팅이 EntryPoint이기에 DllMain으로 실행시 진입점을 변경해줘야 한다.
 DLL로딩시 GRUB메뉴에  module /boot/SampleDll.dll "SAMPLE_DLL" 추가
일반적인 DLL과 달리 힙에 로드시켜 별다른 주소매핑이 필요하지 않지만 힙은 코드적재가아닌 데이터저장을 위한 공간이기에
WIN32에서 힙에 코드실행속성은 없다. 프로세스 공격을 이용한 형태로, 이러한 공격을 막기 위해 프로세스는 DLL코드가 정상적인
영역에 로드됐는지를 확인해야한다. 별도의 페이지 설정과 DLL을 별도의 가상주소에 매핑해야한다. 
 수정을 필요로 하는 주소는 DDL기준로즈주소+IMAGE_BAGE_RELLOCATION->VirtualAddress+WORD배열 각 요소 하위 12비트 값이다.
이 값과 기준주소의 차인 델타값을 더해 주소재배치를 진행한다. 
*/
#include "ServiceFunc.h"

typedef struct _IMAGE_BASE_RELOCATION{
	DWORD VirtualAddress;
	DWORD SizeOfBlock;
} IMAGE_BASE_RELOCATION; 
typedef IMAGE_BASE_RELOCATION UNALIGNED*PIMAGE_BASE_RELOCATION;

static bool PerformBaseRelocation(LOAD_CCL_CONTEXT* ctx, ptrdiff_t delta){//2-1-5-1. LoadDLL내부에서 주소재배치함수 
	//ctx->image_base: 힙에 할당된 베이스 주소
	//ctx->hdr.OptionalHeader.ImageBase: 선호되는 이미지 로드 베이스 주소
	//delta: 위 두 값의 차. 재배치해야되는 값들에 델타값을 더해주어 메모리 주소를 재배치
	unsigned char* codeBase=(unsigned char*)ctx->image_base;//힙에 할당된 DDL 베이스 주소 
	delta=ctx->image_base-ctx->hdr.OptionalHeader.ImageBase;//베이스 주소에서 이미지를 재배치할 값만큼 빼어 차이를 저장한다(일괄적 계산을 위함) 
	
	PIMAGE_DATA_DIRECTORY directory=GET_HEADER_DICTIONARY(ctx->hdr, IMAGE_DIRECTORY_ENTRY_BASERELOC);//재배치 관련 이미지 데이터 디렉토리를 얻는다(ctx_hdr까지 얼마나 변환해야하는지)
	if(directory->Size==0)//변화할 값이 없다면(이미 선호되는 주소라면) 
		return (delta==0);
	
	//재배치 테이블 접근
	relocation=(PIMAGE_BASE_RELOCATION)(coseBase+directory->VirtualAddress);//relocation을 위해 베이스코드주소에 directory값을 더하여 각각의 선호되는 위치정보를 PIMAGE_BASE_RELOCATION구조체로 저장한다(재배치 테이블) 
	for(; relocation->VirtualAddress>0;){
		DWORD i;
		unsigned char* dest=codeBase+relocation->VirtualAddress;//목적지 주소: DDL베이스주소+재배치요소의 가상주소 
		unsigned short* relInfo=(unsigned short*)OffsetPointer(relocation, IMAGE_SIZEOF_BASE_RELOCATION);//수정되어야할 offset값 
		
		for(i=0; i<((relocation->SizeOfBlock-IMAGE_SIZEOF_BASE_RELOCATION)/2); i++, relInfo++){
			int type=*relInfo>>12;//재배치 요소의 상위4비트 재배치 타입값 
			int offset=*relInfo&0xfff;//하위 12비트 오프셋값 
			switch(type){
				case IMAGE_REL_BASED_ABSOLUTE:
					break;
				case IMAGE_REL_BASED_HIGHLOW://수정이 필요하다면 
					{
						DWORD* patchAddrHL=(DWORD*)(dest+offset);//수정되어야할 정보 
						*patchAddrHL+=(DWORD)delta;//델타만큼 더함 
					}
					break;
			}
		}
		relocation=(PIMAGE_BASE_RELOCATION)OffsetPointer(relocation, relocation->SizeOfBlock);//다음 재배치 정보로 넘어간다. 
	}
	return TRUE;
} 

static ELoadDLLResult LoadDLL_CallDLLEntryPoint(LOAD_DLL_CONTEXT* ctx, int flags){//2-1-6. DLL메인 엔트리 실행함수 
	ctx->dll_main=(LPDLLMAIN)(ctx->hdr.OptionalHeader.AddressOfEntryPoint+ctx->image_base);//DLL엔트리 공식: 이미지 로드된 베이스주소+엔틜포인트 오프셋 
	ctx->dll_main(ctx->image_base, DLL_PROCESS_ATTACH, NULL/*(LPVOID)&g_AllocInterface*/);//실행 
}

//어려웡.. 대충 LoadDLLFromMemory내부에서 메서드 주소반환할 때 구조체형태 변수 res에 넣어서 주는듯 . 그래서 생성한 인터페이스를 사용하는거고.. 
ELoadDLLResult LoadDLL(LOAD_DLL_READPROC read_proc, void* read_proc_param, int flags, LOAD_DLL_INFO* info){//2-1-5. res변수에 넣어줌 
	.....
	//DLL헤더로드 
	res=LoadDLL_LoadHeaders(&ctx, read_proc, read_proc_param);//ctx는 LOAD_DLL_CONTEXT_image담긴 구조체 
	//DLL크기만큼 메모리부터 할당 
	res=LoadDLL_AllocateMemory(&ctx, flags);
	//할당한 메모리 내부에 섹션(코드, 데이터, bss)을 넣는다 
	res=LoadDLL_LoadSections(&ctx, read_proc, read_proc_param, flags);
	//주소 재배치 수행 
	PerformBaseRelocation(&ctx, 0);//2-1-5-1. 주소 재배치 수행 
	//메모리 속성 변경 
	res=LoadDLL_SetSectionMemoryProtection(&ctx);
	//DLL메인 엔트리 수행 
	res=LoadDLL_CallDLLEntryPoint(&ctx, flags);//2-1-6. DDLMain실행 
	.....
}

ELoadDLLResult LoadDLLFromMemory(const void* dll_data, size_t dll_size, int flags, LOAD_DLL_INFO* info){//2-1-3. DWORD로 ELoadDLLResult를 받는다. (2)메서드 주소반환 
	LOAD_DLL_FROM_MEMORY_STRUCT ldfms={dll_data, dll_size};
	return LoadDLL((LOAD_DLL_READPROC)&LoadDLLFromMemoryCallback, &ldfms, flags, info);//2-1-4. ELoadDLLResult를 반환 
}

MODULE_HANDLE SkyModuleManager::LoadModuleFromMemory(const char* moduleName){//2-1-1. 모듈 핸들반환 
	LOAD_DLL_INFO* p=FindLoadedModule(moduleName);//핸들. 
	if(p){//로드된 모듈에 존재한다면
		p->refCount++;//참조포인트만 ++하고 핸들반환(실제 Win32에서는 프로세스 락을 더불어 사용) 
		return p;
	}
	
	//새로 로드해야 된다면 
	Module* pModule=FindModule(moduleName);//이름으로 모듈찾기 
	.....
	p=new LOAD_DLL_INFO;//핸들을 만들고 이름설정, 참조카운트 설정 
	strcpy(p->moduleName, moduleName);
	p->refCount=1;
	
	//실제 힙에 로드. 주소 재배치 수행. 리턴타입은 enum ELoadDLlResult. LoadDLL.h
	DWORD res=LoadDLLFromMemory((void*)pModule->ModuleStart, ((size_t)(pModule->ModuleEnd)-(size_t)pModule->ModuleStart), 0, p);//2-1-2. res를 반환하고 res를 LOAD_DDL_INFO로 세팅 
	.....//실패시 오류출력만. 
	SkyConsole::Print("%s Module Loaded\n", moduleName);
	//
	m_moduleList.push_back(p);//로드된 모듈에 추가 
	return p;	
}

//1. 기본 실행되는 EntryPoint말고 DLLMain을 먼저 실행되게 설정(개발) 
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){ 
	//실제 WIN32 DLL은 프로세스 락을 호출(kEnterCriticalSection, kLeaveCriticalSection). SkyOS에서는 참조카운트만 증가(아키텍쳐 확장시 동기화 고려필수) 
	switch(ul_reason_for_call){
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

//2. DLL이 실행되는 예시 
bool TestMemoryModule(const char* moduleName){
	//2-1. 모듈 로딩, 핸들반환 
	MODULE_HANDLE hwnd=SkyModuleManager::GetInstance()->LoadModuleFromMemory(moduleName);//2-1-1. 모듈 로딩 
	//2-2. 모듈이 가지고있는 GetDLLInterface라는 메서드의 주소를 얻는다. (인터페이스를 통해) 
	PGetDLLInterface GetDLLInterface=(PGetDLLInterface)SkyModuleManager::GetInstance()->GetModuleFunction((MODULE_HANDLE)(hwnd), "GetDLLInterface");//(2)
	//2-3. 해당 함수를 호출하여 DLL인터페이스를 얻는다? getter네 
	const DLLInterface* dll_interface=GetDLLInterface();
	//2-4. 인터페이스 내부가 궁금하긴한데, 인터페이스를 통해 함수 호출 및 결과 출력 
	int sum=dll_interface->AddNumber(5,6);
	SkyConsole::Print("AddNumber(5,6): %d\n", sum);
	//2-5. 모듈 해제. 참조포인트만 내려줄듯 
	SkyModuleManager::GetInstance()->UnloadModule((MODULE_HANDLE)(hwnd));
}
