/*
응용프로그램이 최초로 시작되는 부분은 EntryPoint함수이다. 
응용프로그램은 커널API를 직접 호출하는 것이 아닌 wrapperWIN32는 kernel32.dll->ntdll.dll 혹은 gdi32.dll)를 통해 요청을 하는 형태로 진행되며, 
정적 라이브러리 userlib.lib(의 CreateHeap으로 응용프로그램이 요청을 하면 SkyOS에서 SSDT(System Service Descriptor Table)을 참조하여 해당 함수를 반환한다. 
*/
#include "UserAPI.h"

void CreateHeap();
void CreateDefaultHeap();
extern "C" void TerminateProcess();

extern int main();//EntryPoint함수는 userLib에 구현되어 있고 이를 종속성으로 푸가한 프로젝트에서 반드시 main함수는 어딘가에 구현되어야한다. 

extern "C" void EntryPoint(){
	CreateHeap(); 
	int result=main();//메인 엔트리 실행 
	TerminateProcess();
	for(;;);//프로세스 종료까지 대기(컨텍스트 스위칭이 일어날 때 까지)_TeminateProcess에서 현재 task를 종료처리하니, 해당 함수에서 본래 for로 컨텍스트 스위칭이 일어나면 이미 사라져있다. 
}

void CreateHeap(){
	__asm{
		mov eax, 4
		int 0x80; //커널에 서비스 요청(CreateDefaultHeap)
	}
} 
//#define DEFAULT_HEAP_PAGE_COUNT 1MB
#define PAGE_SIZE 4096
void CreateDefaultHeap(){
	kEnterCriticalSection();
	
	Process* pProcess=ProcessManager::GetInstance()->GetCurrentProcess();
	void* pHeapPhys=PhysicalMemoryManager::AllocBlocks( DEFAULT_HEAP_PAGE_COUNT );//1MB의 물리 메모리 블럭을 할당 
	u32int heapAddress=pThread->m_imageBase+pThread->m_imageSize+PAGE_SIZE+PAGE_SIZE*2;//12KB뒤에 힙 1MB를 생성_스레드기준 이미지베이스+이미지크기+3페이지? 뒤를 힙주소로 사용
	heapAddress-=(heapAddress%PAGE_SIZE);//해당 주소에서 PAGE_SIZE만큼을 버려서 PAGE_SIZE단위에 맞춘다. 
	
	for(int i=0; i<DEFAULT_HEAP_PAGE_COUNT; i++){
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddress(
			pProcess->m_pPageDirectory,//프로세스 매니저에서 가져온 현재 프로세스의 PDT에 
			(uint32_t)heapAddress+i*PAGE_SIZE,//힙영역을  
			(uint32_t)pHeapPhys+i*PAGE_SIZE,//물리영역으로 매핑 
			I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER
		);		
	}
	
	memset((void*)heapAddress, 0, DEFAULT_HEAP_PAGE_COUNT*PAGE_SIZE);//가상주소범위의 값을 0으로 초기화한 뒤에 
	pProcess->m_lpHeap=create_heap(//힙을 만들어 프로세스에 등록한다. 
		(u32int)heapAddress,
		(uint32_t)heapAddress+DEFAULT_HEAP_PAGE_COUNT*PAGE_SIZE,
		(uint32_t)heapAddress+DEFAULT_HEAP_PAGE_COUNT*PAGE_SIZE,
		0, 
		0
	);
	kLeaveCriticalSection();
}

extern "C" void TerminateProcess(){
	kEnterCriticalSection();
	Thread* pTask=ProcessManager::GetInstance()->GetCurrentTask();
	Process* pProcess=pTask->m_pParent;
	
	if(pProcess==nullptr || pProces->GetProcessId()==PROC_INVALID_ID){//PID유효성확인 
		SkyConsole::Print("Invalid Process Termination\n");
		kLeaveCriticalSection();
		return;
	}
	
	ProcessManager::GetInstance()->ReserveRemoveProcess(pProcess);//프로세스 매니저는 해당 프로세스에 할당된 리소스를 제거한다. 
	kLeaveCriticalSection();
}
