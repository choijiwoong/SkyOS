#include "GRUB�� �̿��� GUI��ȯ.cpp"
#include "SkyStartOption.h"

class SkyGUISystem{//�Ѱ� �׷��� �̱��� ��ü
	.....
	bool Initialize(multiboot_info* pBootInfo);
	bool Run();
	bool Print(char* pMsg);
	bool Clear();
	.....
	
	private:
		.....
		SkyWindow<SKY_GUI_SYSTEM>* m_pWindow;//SkyConsoleGUI, SkyGUI, SkySVGA�� Ž�� �� �ִ� SkyWindow��ü. SKY_GUI_SYSTEM�� SkyStartOption.h���� SkyGUIConsole�� �����Ǿ��ִ�. 
};
bool SkyGUISystem::Run(){
	if(m_pWidnow)
		m_pWindow->Run();
	return true;
}


DWORD WINAPI SystemGUIProc(LPVOID parameter){
	.....
	systemOn=true;
	
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUZREWAVEGEN);
	.....
	SkyGUISystem::GetInstance()->Initialize(pBootInfo);//�ʱ�ȭ �� ����(SkyGUISystem�� �̱���) 
	SkyGUISystem::GetInstance()->Run();
	return 0;
}

void SkyGUILauncher::Launch(){
	Process* pMainProcess=ProcessManager::GetInstance()->CreateProcessFromMemory("GUISystem", SystemGUIProc, NULL, PROCESS_KERNEL);//GUI�� ���� ���ο� ���μ��� ���� 
	ProcessManager::GetInstance()->CreateProcessFromMemory("ProcessRemover", ProcessRemoverProc, NULL, PROCESS_KERNEL);//���μ������� ���μ��� ���� 
	SkyConsole::Print("Init GUI System....\n");
	
	Thread* pThread=pMainProcess->GetMainThread();//SystemGUIProc�� �۵���ų ���μ������� �����带 ������ jump
	Jump(pThread);
}

//�׷��� ��� ���Ժ� �ڵ�
SkyLauncher pSystemLauncher;
#if SKY_CONSOLE_MODE == 0
	pSystemLauncher=new SkyGUILauncher();
#else
	pSystemLaunchar=new SkyConsoleLauncher();
#endif
	pSystemLauncher->Launch(); 


