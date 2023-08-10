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

Thread* ProcessManager::CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param){//메모리모부터 생성 
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

Thread* ProcessManager::CreateThread(Process* pProcess, FILE* file, LPVOID param){
	unsigned char buf[512];
	IMAGE_DOS_HEADER* dosHeader=0;
	IMAGE_NT_HEADERS* ntHeaders=0;
	unsigned char* memory=0;
	
	int readCnt=StorageManager::GetInstance()->ReadFile(file, buf, 1, 512);//버퍼를 이용하여 file을 읽는다 
	if(readCnt==0)
		return nullptr;
	
	if(!ValidatePEImage(buf)){//유효한 PE파일인지_실행하려는 응용프로그램의 이미지 베이스 검증
		SkyConsole::Print("Invalid PE Format!! %s\n", pProcess->m_processName);
		StorageManager::GetInstance()->CloseFile(file);
		return nullptr;
	}
	
	dosHeader=(IMAGE_DOS_HEADER*)buf;//PE이미지의 헤더를 계산, OptionalHeader의 이미지 베이스와 이미지의 크기를 계산한다?
	ntHeaders=(IMAGE_NT_HEADERS*)(dosHeader->e_lfanew+(uint32_t)buf);
	
	pProcess->m_imageBase=ntHeaders->OptionalHeader.ImageBase;//위의 nt헤더에 계산된 이미지 베이스와 이미지 크기를 프로세스에 저장. 
	pProcess->m_ImageSize=ntHeaders->OptionalHeader.SizeOfImage;
	
	Thread* pThread=new Thread();//새 스레드 생성 
	.....//스레드에 응용프로그램 정보 할당 
	
	int pageRest=0;//파일로부터 읽을 데이터 페이지 수 계산 
	if((pThread->m_imageSize%4096)>0)//4096단위로 읽기가 가능하다면 
		pageRest=1;
	pProcess->m_dwPageCount=(pThread->m_imageSize/4096)+pageRest;//스레드의 이미지 사이즈 갱신 
	
	//물리주소를 가상주소로 매핑 
	unsigned char* physicalMemory=(unsigned char*)PhysicalMemoryManager::AllocBlocks(pProcess->m_dwPageCount);
	
	//1. 커널 스레드 페이지 디렉토리에 가상주소 매핑 
	for(DWORD i=0; i<pProcess->m_dwPageCount; i++){
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddress(
			GetCurrentTask()->m_pParent->GetPageDirecgtory(),//현재 작업중인 태스크의 부모디렉토리(커널 스레드 페이지 디렉토리)에 
			ntHeaders->OptionalHeader.ImageBase+i*PAGE_SIZE,//이미지 베이스에서 4096만큼을 
			(uint32_t)physicalMemory+i*PAGE_SIZE,//물리메모리에 매핑 
			I86_PTE_PRESENT | I86_PTE_WRITABLE//쓰기가능하고 PTE이미지이다 
		);
	}
	
	//생성될 스레드의 페이지 디렉토리에 파일 이미지 매핑 
	for(DWORD i=0; i<pProcess->m_dwPageCount; i++){
		VirtualMemoryManager::MapPhysicalAddressToVirtualAddress(
			pProcess->GetPageDirectory(),//현재 프로세스에서 진행중인 디렉토리에(인자로 들어옴. 스레드가 생성될 현재 디렉토리) 
			ntHeaders->OptionalHeader.ImageBase+i*PAGE_SIZE,
			(uint32_t)physicalMemory+i*PAGE_SIZE,
			I86_PTE_PRESENT | I86_PTE_WRITABLE
		);
	}
	
	memory=(unsigned char*)ntHeaders->OptionalHeader.ImageBase;//이미지 베이스 
	memset(memory, 0, pThread->m_imageSize);//이미지 크기만큼을 0으로 초기화 
	memcpy(memory, buf, 512);//buf의 내용을 memory의 내용을 메모리에 복사 
	
	//파일을 메모리에 로드_위와 같은 방식으로 512단위 
	int fileRest=0;
	if((pThread->m_imageSize%512)!=0)
		fileRest=1;
	
	int readCount=(pThread->m_imageSize/512)+fileRest;
	for(int i=1; i<readCount; i++){
		if(file->_eof==1)
			break;
		
		readCount=StorageManager::GetInstance()->ReadFile(file, memory+512*i, 512, 1);//파일을 메모리에 512씩 읽는다 
	}
	
	//스택생성, 주소공간 매핑
	void* stackAddress=(void*)(g_stackPhysicalAddressPool-PAGE_SIZE*10*kernelStackIndex++);//페이지사이즈를 통해 가용한 스택 주소를 얻는다
	
	//스레드에 ESP, EBP설정, 스택포인터 베이스포인터
	pThread->m_initialStack=(void*)((uint32_t)stackAddress+PAGE_SIZE*10);//스레드에서 사용할 스택정보들 
	pThread->frame.esp=(uint32_t)pThread->m_initialStack;
	pThread->frame.ebp=pThread->frame.esp;
	.....
	
	//파일로드에 사용한 커널태스크 페이지테이블 회수
	for(DWORD i=0; i<pProcess->m_dwPageCount; i++){
		VirtualMemoryManager::UnmapPageTable(//메모리에 있는 데이터를 읽은 것이 아니라, PE이미지를 읽은 것이기에 따로 페이지 매핑을 진행한 것이고 이를 회수.PE파일에 대한 이해는 부록을 참고해 따로 보쟈.. 
			GetCurrentTask()->m_pParent->GetPageDirectory(),//프로세스 페이지는 앞으로 쓸 것이기에 냅두지만, PE이미지는 커널에서 접근해서 가져온 것이기에 이만 회수하는 것 같다. 
			(uint32_t)physicalMemory+i*PAGE_SIZE
		);
	}
	
	//태스크 리스트 추가 
	m_taskList->push_back(pThread);
	
	return pThread;
} 
