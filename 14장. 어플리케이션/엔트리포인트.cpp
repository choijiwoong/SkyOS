/*
�������α׷��� ���ʷ� ���۵Ǵ� �κ��� EntryPoint�Լ��̴�. 
�������α׷��� Ŀ��API�� ���� ȣ���ϴ� ���� �ƴ� wrapperWIN32�� kernel32.dll->ntdll.dll Ȥ�� gdi32.dll)�� ���� ��û�� �ϴ� ���·� ����Ǹ�, 
���� ���̺귯�� userlib.lib(�� CreateHeap���� �������α׷��� ��û�� �ϸ� SkyOS���� SSDT(System Service Descriptor Table)�� �����Ͽ� �ش� �Լ��� ��ȯ�Ѵ�. 
*/
#include "UserAPI.h"

void CreateHeap();
void CreateDefaultHeap();
extern "C" void TerminateProcess();

extern int main();//EntryPoint�Լ��� userLib�� �����Ǿ� �ְ� �̸� ���Ӽ����� Ǫ���� ������Ʈ���� �ݵ�� main�Լ��� ��򰡿� �����Ǿ���Ѵ�. 

extern "C" void EntryPoint(){
	CreateHeap(); 
	int result=main();//���� ��Ʈ�� ���� 
	TerminateProcess();
	for(;;);//���μ��� ������� ���(���ؽ�Ʈ ����Ī�� �Ͼ �� ����)_TeminateProcess���� ���� task�� ����ó���ϴ�, �ش� �Լ����� ���� for�� ���ؽ�Ʈ ����Ī�� �Ͼ�� �̹� ������ִ�. 
}

void CreateHeap(){
	__asm{
		mov eax, 4
		int 0x80; //Ŀ�ο� ���� ��û(CreateDefaultHeap)
	}
} 
//#define DEFAULT_HEAP_PAGE_COUNT 1MB
#define PAGE_SIZE 4096
void CreateDefaultHeap(){
	kEnterCriticalSection();
	
	Process* pProcess=ProcessManager::GetInstance()->GetCurrentProcess();
	void* pHeapPhys=PhysicalMemoryManager::AllocBlocks( DEFAULT_HEAP_PAGE_COUNT );//1MB�� ���� �޸� ���� �Ҵ� 
	u32int heapAddress=pThread->m_imageBase+pThread->m_imageSize+PAGE_SIZE+PAGE_SIZE*2;//12KB�ڿ� �� 1MB�� ����_��������� �̹������̽�+�̹���ũ��+3������? �ڸ� ���ּҷ� ���
	heapAddress-=(heapAddress%PAGE_SIZE);//�ش� �ּҿ��� PAGE_SIZE��ŭ�� ������ PAGE_SIZE������ �����. 
	
	for(int i=0; i<DEFAULT_HEAP_PAGE_COUNT; i++){
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddress(
			pProcess->m_pPageDirectory,//���μ��� �Ŵ������� ������ ���� ���μ����� PDT�� 
			(uint32_t)heapAddress+i*PAGE_SIZE,//��������  
			(uint32_t)pHeapPhys+i*PAGE_SIZE,//������������ ���� 
			I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER
		);		
	}
	
	memset((void*)heapAddress, 0, DEFAULT_HEAP_PAGE_COUNT*PAGE_SIZE);//�����ּҹ����� ���� 0���� �ʱ�ȭ�� �ڿ� 
	pProcess->m_lpHeap=create_heap(//���� ����� ���μ����� ����Ѵ�. 
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
	
	if(pProcess==nullptr || pProces->GetProcessId()==PROC_INVALID_ID){//PID��ȿ��Ȯ�� 
		SkyConsole::Print("Invalid Process Termination\n");
		kLeaveCriticalSection();
		return;
	}
	
	ProcessManager::GetInstance()->ReserveRemoveProcess(pProcess);//���μ��� �Ŵ����� �ش� ���μ����� �Ҵ�� ���ҽ��� �����Ѵ�. 
	kLeaveCriticalSection();
}
