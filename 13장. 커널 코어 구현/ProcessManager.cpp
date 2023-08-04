#include <iostream>

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
