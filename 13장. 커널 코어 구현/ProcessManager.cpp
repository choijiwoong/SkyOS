#include <iostream>

#pragma pack (push, 1)
typedef struct registers{ 
	u32int ds, es, fs, gs;//데이터 세그먼트 셀렉터
	u32int edi, esi, ebp, esp, ebx, edx, ecx, eas;//PUSHAD
	u32int eip, cs, eflags, esp, ss;//CPU가 자동으로 채우는 데이터 
}registers_t;
#pragma pack (pop) 

class Thread{
	public:
		int			m_taskState;//Init, Running, Stop, Terminate
		UINT32		m_dwPriority;
		int			m_waitingTime;//CPU선점시간. 0이 되면 다른 태스크로 전환된다. 
	
		Process*	m_pParent;
		LPVOID		m_startParam;
		void*		m_initialStack;//베이스 스택 주소 
	
		uint32_t 	m_esp;//스택포인터 
		UINT32		m_stackLimit;//스택 크기(베이스) 
		trapFrame	frame;
	
		uint32_t	m_imageBase;//파일에서 코드를 로드할 때, 메모리에 로드된 베이스 주소 
		uint32_t	m_imageSize;//파일의 크기 
	
		registers_t m_contextSnapshot;//컨텍스트 스위칭을 위해, 다른작업에서 현재 스레드로 돌아올 때, 수행했던 직전 상황을 복원할 수 있어야 한다. 
	
		void SetThreadId(DWORD threadId){ m_threadId=threadId; }
		DWORD GetThreadId(){ return m_threadId; }
	
		void* m_lpTLS=nullptr;//로컬 공간. 스레드간 데이터 경합이나 동시성 문제를 우회하기 위한 추가공간. 
	
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
	pThread->frame.eip=(uint32_t)lpStartAddress;//EIP_스레드의 엔트리를 나타내는 함수 주소. 
	pThread->frame.flags=0x200;//플래그 
	pThread->m_startParam=param;//파라미터_EIP에 있는 시작주소를 실행하며 사용할 플래그와 매개변수 지정인가? 위엔 메타정보이고. 실제 수행할 작업의 정보는 trapFrame타입에 저장하나보네 
	
	//stack
	void* stackAddress=(void*)(g_stackPhysicalAddressPool - STACK_SIZE * kernelStackIndex++);//사용할 스택의 크기만큼 스택의 시작주소__가용한 스택물리메모리를 스택 크기만큼 스레드마다 할당하기 위함인듯 
	
	//ESP, EBP가 스택을 가리킨다. 
	pThread->m_initialStack=(void*)((uint32_t)stackAddress+STACK_SIZE);
	pThread->frame.esp=(uint32_t)pThread->m_initialStack;
	pThread->frame.ebp=pThread->frame.esp;
	
	m_taskList->push_back(pThread);//태스크리스트에 스레드 추가 
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
		static ProcessManager* GetInstance(){//스레드 안전하지 않기에 GetInstance함수 호출시마다 lock필수. 혹은 코드 초반에 미리 생성해두기(if문으로 아예 안빠지도록) 
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
		int m_nextThreadId;//다음 스레드에 붙일 아이디 
		
		Loader* m_pKernelProcessLoader;//커널모드 로더 
		Loader* m_pUserProcessLoader;//유저모드 로더 
		
		ProcessList m_processList;//프로세스 
		TaskList m_taskList;//실행중인태스크리스트 
		TaskList m_terminatedTaskList;//종료된태스크 
		Thread* m_pCurrentTask;//실행중인태스크 ?(아마 포그라운드, 위에껀 백그라운드 일듯) 
};

Process* ProcessManager::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType){
	Process* pProcess=nullptr;
	//프로세스 객체 생성(유저모드인지, 커널모드인지에 따라 다름 
	if(processType==PROCESS_KERNEL)
		pProcess=m_pKernelProcessLoader->CreateProcessFromMemory(appName, lpStartAddress, param);//웬만하면 Kernel모드 만 사용됨. 
	else
		pProcess=m_pUserProcessLoader->CreateProcessFromMemory(appName, lpStartAddress, param);
	
	//스레드 생성, 방금 생성된 프로세스에 추가
	Thread* pThread=CreateThread(pProcess, lpStartAddress, param);
	bool result=pProcess->AddMainThread(pThread);
	result=AddProcess(pProcess);//현재 프로세스를 프로세스 리스트에 추가 
	return pProcess; //현재 프로세스를 반환 
}
