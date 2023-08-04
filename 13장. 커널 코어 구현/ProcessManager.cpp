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

Thread* ProcessManager::CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param){
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
