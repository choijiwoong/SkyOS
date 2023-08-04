#include "ProcessManager.cpp"

class Loader: public ProcessManager{};

class KernelProcessLoader: public Loader{};
class UserProcessLoader: public Loader{};

//메모리를 이용하여 프로세스 생성 
Process* KernelProcessLoader::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param){
	Process* pProcess=new Process();//프로세스 생성 및 아이디 할당
	pProcess->SetProcessId(GetNextProcessId());
	
	PageDirectory* pPageDirectory=nullptr;
	PhysicalMemoryManager::EnablePaging(false);
	
	pPageDirectory=VirtualMemoryManager::CreateCommonPageDirectory();//별도의 페이징사용을 위함 
	if(pPageDirectory==nullptr){
		PhysicalMemoryManager::EnablePaging(true);
		return nullptr;
	}
	
	HeapManager::MapHeapToAddressSpace(pPageDirectory);//힙공간을 페이지디렉토리에 매핑
	if(SkyGUISystem::GetInstance()->GUIEnable()==true){
		//그래픽 모드라면 그래픽 버퍼 주소를 페이지 디렉토리에 매핑 
	} 
	
	//페이징 활성화 및 프로세스에 페이지디렉토리 저장 
	PhysicalMemoryManager::EnablePaging(true);
	pProcess->SetPageDirectory(pPageDirectory);//페이징은 가상메모리공간을 사용하는 것이고, 별도의 페이지 디렉토리를 만들어 프로세스에 할당시켰다. 
	
	//프로세스 객체 초기세팅 
	pProcess->m_dwRunState=TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);
	pProcess->m_dwProcessType=PROCESS_KERNEL;
	//pProcess->m_dwPriority=1;
	
	return pProcess; 
}
