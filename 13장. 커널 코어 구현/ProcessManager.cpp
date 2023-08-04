#include <iostream>

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
