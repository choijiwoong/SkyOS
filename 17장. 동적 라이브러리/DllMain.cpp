/*
DLL������ �ھ������� �׻� 0x100000�� �ε�Ǵ� �Ͱ� �޸� ��ȣ�Ǵ� ������ �����Ǿ����� �ʴ�.
������ �⺻ ������ EntryPoint�̱⿡ DllMain���� ����� �������� ��������� �Ѵ�.
 DLL�ε��� GRUB�޴���  module /boot/SampleDll.dll "SAMPLE_DLL" �߰�
�Ϲ����� DLL�� �޸� ���� �ε���� ���ٸ� �ּҸ����� �ʿ����� ������ ���� �ڵ����簡�ƴ� ������������ ���� �����̱⿡
WIN32���� ���� �ڵ����Ӽ��� ����. ���μ��� ������ �̿��� ���·�, �̷��� ������ ���� ���� ���μ����� DLL�ڵ尡 ��������
������ �ε�ƴ����� Ȯ���ؾ��Ѵ�. ������ ������ ������ DLL�� ������ �����ּҿ� �����ؾ��Ѵ�. 
 ������ �ʿ�� �ϴ� �ּҴ� DDL���ط����ּ�+IMAGE_BAGE_RELLOCATION->VirtualAddress+WORD�迭 �� ��� ���� 12��Ʈ ���̴�.
�� ���� �����ּ��� ���� ��Ÿ���� ���� �ּ����ġ�� �����Ѵ�. 
*/
#include "ServiceFunc.h"

typedef struct _IMAGE_BASE_RELOCATION{
	DWORD VirtualAddress;
	DWORD SizeOfBlock;
} IMAGE_BASE_RELOCATION; 
typedef IMAGE_BASE_RELOCATION UNALIGNED*PIMAGE_BASE_RELOCATION;

static bool PerformBaseRelocation(LOAD_CCL_CONTEXT* ctx, ptrdiff_t delta){//2-1-5-1. LoadDLL���ο��� �ּ����ġ�Լ� 
	//ctx->image_base: ���� �Ҵ�� ���̽� �ּ�
	//ctx->hdr.OptionalHeader.ImageBase: ��ȣ�Ǵ� �̹��� �ε� ���̽� �ּ�
	//delta: �� �� ���� ��. ���ġ�ؾߵǴ� ���鿡 ��Ÿ���� �����־� �޸� �ּҸ� ���ġ
	unsigned char* codeBase=(unsigned char*)ctx->image_base;//���� �Ҵ�� DDL ���̽� �ּ� 
	delta=ctx->image_base-ctx->hdr.OptionalHeader.ImageBase;//���̽� �ּҿ��� �̹����� ���ġ�� ����ŭ ���� ���̸� �����Ѵ�(�ϰ��� ����� ����) 
	
	PIMAGE_DATA_DIRECTORY directory=GET_HEADER_DICTIONARY(ctx->hdr, IMAGE_DIRECTORY_ENTRY_BASERELOC);//���ġ ���� �̹��� ������ ���丮�� ��´�(ctx_hdr���� �󸶳� ��ȯ�ؾ��ϴ���)
	if(directory->Size==0)//��ȭ�� ���� ���ٸ�(�̹� ��ȣ�Ǵ� �ּҶ��) 
		return (delta==0);
	
	//���ġ ���̺� ����
	relocation=(PIMAGE_BASE_RELOCATION)(coseBase+directory->VirtualAddress);//relocation�� ���� ���̽��ڵ��ּҿ� directory���� ���Ͽ� ������ ��ȣ�Ǵ� ��ġ������ PIMAGE_BASE_RELOCATION����ü�� �����Ѵ�(���ġ ���̺�) 
	for(; relocation->VirtualAddress>0;){
		DWORD i;
		unsigned char* dest=codeBase+relocation->VirtualAddress;//������ �ּ�: DDL���̽��ּ�+���ġ����� �����ּ� 
		unsigned short* relInfo=(unsigned short*)OffsetPointer(relocation, IMAGE_SIZEOF_BASE_RELOCATION);//�����Ǿ���� offset�� 
		
		for(i=0; i<((relocation->SizeOfBlock-IMAGE_SIZEOF_BASE_RELOCATION)/2); i++, relInfo++){
			int type=*relInfo>>12;//���ġ ����� ����4��Ʈ ���ġ Ÿ�԰� 
			int offset=*relInfo&0xfff;//���� 12��Ʈ �����°� 
			switch(type){
				case IMAGE_REL_BASED_ABSOLUTE:
					break;
				case IMAGE_REL_BASED_HIGHLOW://������ �ʿ��ϴٸ� 
					{
						DWORD* patchAddrHL=(DWORD*)(dest+offset);//�����Ǿ���� ���� 
						*patchAddrHL+=(DWORD)delta;//��Ÿ��ŭ ���� 
					}
					break;
			}
		}
		relocation=(PIMAGE_BASE_RELOCATION)OffsetPointer(relocation, relocation->SizeOfBlock);//���� ���ġ ������ �Ѿ��. 
	}
	return TRUE;
} 

static ELoadDLLResult LoadDLL_CallDLLEntryPoint(LOAD_DLL_CONTEXT* ctx, int flags){//2-1-6. DLL���� ��Ʈ�� �����Լ� 
	ctx->dll_main=(LPDLLMAIN)(ctx->hdr.OptionalHeader.AddressOfEntryPoint+ctx->image_base);//DLL��Ʈ�� ����: �̹��� �ε�� ���̽��ּ�+��ƹ����Ʈ ������ 
	ctx->dll_main(ctx->image_base, DLL_PROCESS_ATTACH, NULL/*(LPVOID)&g_AllocInterface*/);//���� 
}

//�����.. ���� LoadDLLFromMemory���ο��� �޼��� �ּҹ�ȯ�� �� ����ü���� ���� res�� �־ �ִµ� . �׷��� ������ �������̽��� ����ϴ°Ű�.. 
ELoadDLLResult LoadDLL(LOAD_DLL_READPROC read_proc, void* read_proc_param, int flags, LOAD_DLL_INFO* info){//2-1-5. res������ �־��� 
	.....
	//DLL����ε� 
	res=LoadDLL_LoadHeaders(&ctx, read_proc, read_proc_param);//ctx�� LOAD_DLL_CONTEXT_image��� ����ü 
	//DLLũ�⸸ŭ �޸𸮺��� �Ҵ� 
	res=LoadDLL_AllocateMemory(&ctx, flags);
	//�Ҵ��� �޸� ���ο� ����(�ڵ�, ������, bss)�� �ִ´� 
	res=LoadDLL_LoadSections(&ctx, read_proc, read_proc_param, flags);
	//�ּ� ���ġ ���� 
	PerformBaseRelocation(&ctx, 0);//2-1-5-1. �ּ� ���ġ ���� 
	//�޸� �Ӽ� ���� 
	res=LoadDLL_SetSectionMemoryProtection(&ctx);
	//DLL���� ��Ʈ�� ���� 
	res=LoadDLL_CallDLLEntryPoint(&ctx, flags);//2-1-6. DDLMain���� 
	.....
}

ELoadDLLResult LoadDLLFromMemory(const void* dll_data, size_t dll_size, int flags, LOAD_DLL_INFO* info){//2-1-3. DWORD�� ELoadDLLResult�� �޴´�. (2)�޼��� �ּҹ�ȯ 
	LOAD_DLL_FROM_MEMORY_STRUCT ldfms={dll_data, dll_size};
	return LoadDLL((LOAD_DLL_READPROC)&LoadDLLFromMemoryCallback, &ldfms, flags, info);//2-1-4. ELoadDLLResult�� ��ȯ 
}

MODULE_HANDLE SkyModuleManager::LoadModuleFromMemory(const char* moduleName){//2-1-1. ��� �ڵ��ȯ 
	LOAD_DLL_INFO* p=FindLoadedModule(moduleName);//�ڵ�. 
	if(p){//�ε�� ��⿡ �����Ѵٸ�
		p->refCount++;//��������Ʈ�� ++�ϰ� �ڵ��ȯ(���� Win32������ ���μ��� ���� ���Ҿ� ���) 
		return p;
	}
	
	//���� �ε��ؾ� �ȴٸ� 
	Module* pModule=FindModule(moduleName);//�̸����� ���ã�� 
	.....
	p=new LOAD_DLL_INFO;//�ڵ��� ����� �̸�����, ����ī��Ʈ ���� 
	strcpy(p->moduleName, moduleName);
	p->refCount=1;
	
	//���� ���� �ε�. �ּ� ���ġ ����. ����Ÿ���� enum ELoadDLlResult. LoadDLL.h
	DWORD res=LoadDLLFromMemory((void*)pModule->ModuleStart, ((size_t)(pModule->ModuleEnd)-(size_t)pModule->ModuleStart), 0, p);//2-1-2. res�� ��ȯ�ϰ� res�� LOAD_DDL_INFO�� ���� 
	.....//���н� ������¸�. 
	SkyConsole::Print("%s Module Loaded\n", moduleName);
	//
	m_moduleList.push_back(p);//�ε�� ��⿡ �߰� 
	return p;	
}

//1. �⺻ ����Ǵ� EntryPoint���� DLLMain�� ���� ����ǰ� ����(����) 
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){ 
	//���� WIN32 DLL�� ���μ��� ���� ȣ��(kEnterCriticalSection, kLeaveCriticalSection). SkyOS������ ����ī��Ʈ�� ����(��Ű���� Ȯ��� ����ȭ ����ʼ�) 
	switch(ul_reason_for_call){
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

//2. DLL�� ����Ǵ� ���� 
bool TestMemoryModule(const char* moduleName){
	//2-1. ��� �ε�, �ڵ��ȯ 
	MODULE_HANDLE hwnd=SkyModuleManager::GetInstance()->LoadModuleFromMemory(moduleName);//2-1-1. ��� �ε� 
	//2-2. ����� �������ִ� GetDLLInterface��� �޼����� �ּҸ� ��´�. (�������̽��� ����) 
	PGetDLLInterface GetDLLInterface=(PGetDLLInterface)SkyModuleManager::GetInstance()->GetModuleFunction((MODULE_HANDLE)(hwnd), "GetDLLInterface");//(2)
	//2-3. �ش� �Լ��� ȣ���Ͽ� DLL�������̽��� ��´�? getter�� 
	const DLLInterface* dll_interface=GetDLLInterface();
	//2-4. �������̽� ���ΰ� �ñ��ϱ��ѵ�, �������̽��� ���� �Լ� ȣ�� �� ��� ��� 
	int sum=dll_interface->AddNumber(5,6);
	SkyConsole::Print("AddNumber(5,6): %d\n", sum);
	//2-5. ��� ����. ��������Ʈ�� �����ٵ� 
	SkyModuleManager::GetInstance()->UnloadModule((MODULE_HANDLE)(hwnd));
}
