#include "GRUB를 이용한 GUI전환.cpp"
#include "SkyStartOption.h"

class SkyGUISystem{//총괄 그래픽 싱글턴 객체
	.....
	bool Initialize(multiboot_info* pBootInfo);
	bool Run();
	bool Print(char* pMsg);
	bool Clear();
	.....
	
	private:
		.....
		SkyWindow<SKY_GUI_SYSTEM>* m_pWindow;//SkyConsoleGUI, SkyGUI, SkySVGA를 탐을 수 있는 SkyWindow객체. SKY_GUI_SYSTEM은 SkyStartOption.h에서 SkyGUIConsole로 설정되어있다. 
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
	SkyGUISystem::GetInstance()->Initialize(pBootInfo);//초기화 및 실행(SkyGUISystem은 싱글턴) 
	SkyGUISystem::GetInstance()->Run();
	return 0;
}

void SkyGUILauncher::Launch(){
	Process* pMainProcess=ProcessManager::GetInstance()->CreateProcessFromMemory("GUISystem", SystemGUIProc, NULL, PROCESS_KERNEL);//GUI를 위한 새로운 프로세스 생성 
	ProcessManager::GetInstance()->CreateProcessFromMemory("ProcessRemover", ProcessRemoverProc, NULL, PROCESS_KERNEL);//프로세스제거 프로세스 생성 
	SkyConsole::Print("Init GUI System....\n");
	
	Thread* pThread=pMainProcess->GetMainThread();//SystemGUIProc를 작동시킬 프로세스에서 스레드를 가져와 jump
	Jump(pThread);
}

//그래픽 모드 진입부 코드
SkyLauncher pSystemLauncher;
#if SKY_CONSOLE_MODE == 0
	pSystemLauncher=new SkyGUILauncher();
#else
	pSystemLaunchar=new SkyConsoleLauncher();
#endif
	pSystemLauncher->Launch(); 


