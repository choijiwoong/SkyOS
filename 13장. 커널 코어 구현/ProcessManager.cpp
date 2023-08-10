#include <iostream>

#pragma pack (push, 1)
typedef struct registers{ 
	u32int ds, es, fs, gs;//������ ���׸�Ʈ ������
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eas;//PUSHAD
	u32int eip, cs, eflags, esp, ss;//CPU�� �ڵ����� ä��� ������ 
}registers_t;
#pragma pack (pop) 

class Thread{
	public:
		int			m_taskState;//Init, Running, Stop, Terminate
		UINT32		m_dwPriority;
		int			m_waitingTime;//CPU�����ð�. 0�� �Ǹ� �ٸ� �½�ũ�� ��ȯ�ȴ�. 
	
		Process*	m_pParent;
		LPVOID		m_startParam;
		void*		m_initialStack;//���̽� ���� �ּ� 
	
		uint32_t 	m_esp;//���������� 
		UINT32		m_stackLimit;//���� ũ��(���̽�) 
		trapFrame	frame;
	
		uint32_t	m_imageBase;//���Ͽ��� �ڵ带 �ε��� ��, �޸𸮿� �ε�� ���̽� �ּ� 
		uint32_t	m_imageSize;//������ ũ�� 
	
		registers_t m_contextSnapshot;//���ؽ�Ʈ ����Ī�� ����, �ٸ��۾����� ���� ������� ���ƿ� ��, �����ߴ� ���� ��Ȳ�� ������ �� �־�� �Ѵ�. 
	
		void SetThreadId(DWORD threadId){ m_threadId=threadId; }
		DWORD GetThreadId(){ return m_threadId; }
	
		void* m_lpTLS=nullptr;//���� ����. �����尣 ������ �����̳� ���ü� ������ ��ȸ�ϱ� ���� �߰�����. 
	
	protected:
		DWORD m_threadId;
};

class ProcessManager{
	public:
		.....
		typedef map<int, Process*> ProcessList;
		typedef list<Thread*> TaskList;
	
		ProcessList* GetProcessList(){ return &m_processList; }
		TaskList* GetTaskList(){ return &m_taskList; }
	
		//singleton
		static ProcessManager* GetInstance(){//������ �������� �ʱ⿡ GetInstance�Լ� ȣ��ø��� lock�ʼ�. Ȥ�� �ڵ� �ʹݿ� �̸� �����صα�(if������ �ƿ� �Ⱥ�������) 
			if(m_processManager==nullptr)
				m_processManager=new ProcessManager();
			return m_processManager;
		}
	
		//create process
		Process* CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType);
		Process* CreateProcessFromFile(char* appName, void* param, UINT32 processType);
		//create thread
		Thread* CreateThread(Process* pProcess, FILE* pFile, LPVOID param);
		Thread* CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);
	
		Process* FindProcess(int processId);
		bool RemoveProcess(int processId);
		Thread* FindThread(DWORD taskId);
		.....
	
	private:
		bool AddProcess(Process* pProcess);
		
	private:
		static ProcessManager* m_processManager;//singleton object
		int m_nextThreadId;//���� �����忡 ���� ���̵� 
		
		Loader* m_pKernelProcessLoader;//Ŀ�θ�� �δ� 
		Loader* m_pUserProcessLoader;//������� �δ� 
		
		ProcessList m_processList;//���μ��� 
		TaskList m_taskList;//���������½�ũ����Ʈ 
		TaskList m_terminatedTaskList;//������½�ũ 
		Thread* m_pCurrentTask;//���������½�ũ ?(�Ƹ� ���׶���, ������ ��׶��� �ϵ�) 
};

Process* ProcessManager::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType){
	Process* pProcess=nullptr;
	//���μ��� ��ü ����(�����������, Ŀ�θ�������� ���� �ٸ� 
	if(processType==PROCESS_KERNEL)
		pProcess=m_pKernelProcessLoader->CreateProcessFromMemory(appName, lpStartAddress, param);//�����ϸ� Kernel��� �� ����. 
	else
		pProcess=m_pUserProcessLoader->CreateProcessFromMemory(appName, lpStartAddress, param);
	
	//������ ����, ��� ������ ���μ����� �߰�
	Thread* pThread=CreateThread(pProcess, lpStartAddress, param);
	bool result=pProcess->AddMainThread(pThread);
	result=AddProcess(pProcess);//���� ���μ����� ���μ��� ����Ʈ�� �߰� 
	return pProcess; //���� ���μ����� ��ȯ 
}

Thread* ProcessManager::CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param){//�޸𸮸���� ���� 
	Thread* pThread=new Thread();
	pThread->m_pParent=pProcess;
	pThread->SetThreadId(m_nextThreadId++);
	pThread->m_dwPriority=1;
	pThread->m_taskState=TASK_STATE_INIT;
	pThread->m_waitingTime=TASK_RUNNING_TIME;
	pThread->m_stackLimit=STACK_SIZE;
	pThread->m_imageBase=0;
	pThread->m_imageSize=0;
	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip=(uint32_t)lpStartAddress;//EIP_�������� ��Ʈ���� ��Ÿ���� �Լ� �ּ�. 
	pThread->frame.flags=0x200;//�÷��� 
	pThread->m_startParam=param;//�Ķ����_EIP�� �ִ� �����ּҸ� �����ϸ� ����� �÷��׿� �Ű����� �����ΰ�? ���� ��Ÿ�����̰�. ���� ������ �۾��� ������ trapFrameŸ�Կ� �����ϳ����� 
	
	//stack
	void* stackAddress=(void*)(g_stackPhysicalAddressPool - STACK_SIZE * kernelStackIndex++);//����� ������ ũ�⸸ŭ ������ �����ּ�__������ ���ù����޸𸮸� ���� ũ�⸸ŭ �����帶�� �Ҵ��ϱ� �����ε� 
	
	//ESP, EBP�� ������ ����Ų��. 
	pThread->m_initialStack=(void*)((uint32_t)stackAddress+STACK_SIZE);
	pThread->frame.esp=(uint32_t)pThread->m_initialStack;
	pThread->frame.ebp=pThread->frame.esp;
	
	m_taskList->push_back(pThread);//�½�ũ����Ʈ�� ������ �߰� 
	return pThread;
}

Thread* ProcessManager::CreateThread(Process* pProcess, FILE* file, LPVOID param){
	unsigned char buf[512];
	IMAGE_DOS_HEADER* dosHeader=0;
	IMAGE_NT_HEADERS* ntHeaders=0;
	unsigned char* memory=0;
	
	int readCnt=StorageManager::GetInstance()->ReadFile(file, buf, 1, 512);//���۸� �̿��Ͽ� file�� �д´� 
	if(readCnt==0)
		return nullptr;
	
	if(!ValidatePEImage(buf)){//��ȿ�� PE��������_�����Ϸ��� �������α׷��� �̹��� ���̽� ����
		SkyConsole::Print("Invalid PE Format!! %s\n", pProcess->m_processName);
		StorageManager::GetInstance()->CloseFile(file);
		return nullptr;
	}
	
	dosHeader=(IMAGE_DOS_HEADER*)buf;//PE�̹����� ����� ���, OptionalHeader�� �̹��� ���̽��� �̹����� ũ�⸦ ����Ѵ�?
	ntHeaders=(IMAGE_NT_HEADERS*)(dosHeader->e_lfanew+(uint32_t)buf);
	
	pProcess->m_imageBase=ntHeaders->OptionalHeader.ImageBase;//���� nt����� ���� �̹��� ���̽��� �̹��� ũ�⸦ ���μ����� ����. 
	pProcess->m_ImageSize=ntHeaders->OptionalHeader.SizeOfImage;
	
	Thread* pThread=new Thread();//�� ������ ���� 
	.....//�����忡 �������α׷� ���� �Ҵ� 
	
	int pageRest=0;//���Ϸκ��� ���� ������ ������ �� ��� 
	if((pThread->m_imageSize%4096)>0)//4096������ �бⰡ �����ϴٸ� 
		pageRest=1;
	pProcess->m_dwPageCount=(pThread->m_imageSize/4096)+pageRest;//�������� �̹��� ������ ���� 
	
	//�����ּҸ� �����ּҷ� ���� 
	unsigned char* physicalMemory=(unsigned char*)PhysicalMemoryManager::AllocBlocks(pProcess->m_dwPageCount);
	
	//1. Ŀ�� ������ ������ ���丮�� �����ּ� ���� 
	for(DWORD i=0; i<pProcess->m_dwPageCount; i++){
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddress(
			GetCurrentTask()->m_pParent->GetPageDirecgtory(),//���� �۾����� �½�ũ�� �θ���丮(Ŀ�� ������ ������ ���丮)�� 
			ntHeaders->OptionalHeader.ImageBase+i*PAGE_SIZE,//�̹��� ���̽����� 4096��ŭ�� 
			(uint32_t)physicalMemory+i*PAGE_SIZE,//�����޸𸮿� ���� 
			I86_PTE_PRESENT | I86_PTE_WRITABLE//���Ⱑ���ϰ� PTE�̹����̴� 
		);
	}
	
	//������ �������� ������ ���丮�� ���� �̹��� ���� 
	for(DWORD i=0; i<pProcess->m_dwPageCount; i++){
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddress(
			pProcess->GetPageDirectory(),//���� ���μ������� �������� ���丮��(���ڷ� ����. �����尡 ������ ���� ���丮) 
			ntHeaders->OptionalHeader.ImageBase+i*PAGE_SIZE,
			(uint32_t)physicalMemory+i*PAGE_SIZE,
			I86_PTE_PRESENT | I86_PTE_WRITABLE
		);
	}
	
	memory=(unsigned char*)ntHeaders->OptionalHeader.ImageBase;//�̹��� ���̽� 
	memset(memory, 0, pThread->m_imageSize);//�̹��� ũ�⸸ŭ�� 0���� �ʱ�ȭ 
	memcpy(memory, buf, 512);//buf�� ������ memory�� ������ �޸𸮿� ���� 
	
	//������ �޸𸮿� �ε�_���� ���� ������� 512���� 
	int fileRest=0;
	if((pThread->m_imageSize%512)!=0)
		fileRest=1;
	
	int readCount=(pThread->m_imageSize/512)+fileRest;
	for(int i=1; i<readCount; i++){
		if(file->_eof==1)
			break;
		
		readCount=StorageManager::GetInstance()->ReadFile(file, memory+512*i, 512, 1);//������ �޸𸮿� 512�� �д´� 
	}
	
	//���û���, �ּҰ��� ����
	void* stackAddress=(void*)(g_stackPhysicalAddressPool-PAGE_SIZE*10*kernelStackIndex++);//����������� ���� ������ ���� �ּҸ� ��´�
	
	//�����忡 ESP, EBP����, ���������� ���̽�������
	pThread->m_initialStack=(void*)((uint32_t)stackAddress+PAGE_SIZE*10);//�����忡�� ����� ���������� 
	pThread->frame.esp=(uint32_t)pThread->m_initialStack;
	pThread->frame.ebp=pThread->frame.esp;
	.....
	
	//���Ϸε忡 ����� Ŀ���½�ũ ���������̺� ȸ��
	for(DWORD i=0; i<pProcess->m_dwPageCount; i++){
		VirtualMemoryManager::UnmapPageTable(//�޸𸮿� �ִ� �����͸� ���� ���� �ƴ϶�, PE�̹����� ���� ���̱⿡ ���� ������ ������ ������ ���̰� �̸� ȸ��.PE���Ͽ� ���� ���ش� �η��� ������ ���� ����.. 
			GetCurrentTask()->m_pParent->GetPageDirectory(),//���μ��� �������� ������ �� ���̱⿡ ��������, PE�̹����� Ŀ�ο��� �����ؼ� ������ ���̱⿡ �̸� ȸ���ϴ� �� ����. 
			(uint32_t)physicalMemory+i*PAGE_SIZE
		);
	}
	
	//�½�ũ ����Ʈ �߰� 
	m_taskList->push_back(pThread);
	
	return pThread;
} 
