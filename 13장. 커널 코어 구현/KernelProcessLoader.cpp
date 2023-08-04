#include "ProcessManager.cpp"

class Loader: public ProcessManager{};

class KernelProcessLoader: public Loader{};
class UserProcessLoader: public Loader{};

//�޸𸮸� �̿��Ͽ� ���μ��� ���� 
Process* KernelProcessLoader::CreateProcessFromMemory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param){
	Process* pProcess=new Process();//���μ��� ���� �� ���̵� �Ҵ�
	pProcess->SetProcessId(GetNextProcessId());
	
	PageDirectory* pPageDirectory=nullptr;
	PhysicalMemoryManager::EnablePaging(false);
	
	pPageDirectory=VirtualMemoryManager::CreateCommonPageDirectory();//������ ����¡����� ���� 
	if(pPageDirectory==nullptr){
		PhysicalMemoryManager::EnablePaging(true);
		return nullptr;
	}
	
	HeapManager::MapHeapToAddressSpace(pPageDirectory);//�������� ���������丮�� ����
	if(SkyGUISystem::GetInstance()->GUIEnable()==true){
		//�׷��� ����� �׷��� ���� �ּҸ� ������ ���丮�� ���� 
	} 
	
	//����¡ Ȱ��ȭ �� ���μ����� ���������丮 ���� 
	PhysicalMemoryManager::EnablePaging(true);
	pProcess->SetPageDirectory(pPageDirectory);//����¡�� ����޸𸮰����� ����ϴ� ���̰�, ������ ������ ���丮�� ����� ���μ����� �Ҵ���״�. 
	
	//���μ��� ��ü �ʱ⼼�� 
	pProcess->m_dwRunState=TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);
	pProcess->m_dwProcessType=PROCESS_KERNEL;
	//pProcess->m_dwPriority=1;
	
	return pProcess; 
}
