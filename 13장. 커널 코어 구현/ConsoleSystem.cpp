//SkyOS의 콘솔시스템: 유저입력을 받는 콘솔 매니저, 종료된 프로세스를 정리하는 프로세스 리무버, 멀티태스킹검증하는 WatchDog

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack);
DWORD WINAPI SystemConsoleProc(LPVOID parameter);
void NativeConsole();
DWORD WINAPI WatchDogProc(LPVOID parameter);
DWORD WINAPI ProcessRemoverProc(LPVOID parameter); 

void StartConsoleSystem(){
	kEnterCriticalSection();
	
	//기본 프로세스 생성 
	Process* pProcess=ProcessManager::GetInstance()->CreateProcessFromMemory("ConsoleSystem", SystemConsoleProc, NULL, PROCESS_KERNEL);
	ProcessManager::GetInstance()->CreateProcessFromMemory("WatchDog", WatchDogProc, NULL, PROCESS_KERNEL);
	ProcessManager::GetInstance()->CreateProcessFromMemory("ProcessRemover", ProcessRemoverProc, NULL, PROCESS_KERNEL);
	if(pProcess==nullptr)
		HaltSystem("Console Creation Fail!!");
	
	SkyConsole::Print("Init Console....\n");
	
	//콘솔프로세스의 메인 스레드를 가져와 
	Thread* pThread=pProcess->GetMainThread();
	if(pThread==nullptr)
		HaltSystem("Console Creation Fail!!");
 
	pThread->m_taskState=TASK_STATE_RUNNING;//사용중표시 
	
	//엔트리포인스와 스택을 설정하고 
	int entryPoint=(int)pThread->frame.eip;
	unsigned int procStack=pThread->frame.esp;
	
	kLeaveCriticalSection();
	
	//점프한다_콘솔시스템으로 
	JumpToNewKernelEntry(entryPoint, procStack);
}

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack){
	__asm{
		MOV AX, 0x10; //GDT에서 0x10부터 커널 데이터 디스크립터이기에 세그먼트 셀렉터 레지스터를 초기화. 
		MOV DS, AX
		MOV ES, AX
		MOV FS, AX
		MOV GS, AX
		
		MOV ESP, procStack
		PUSH 0; //ss?
		PUSH 0; //EBP
		PUSH 0x200; //EFLAGS
		PUSH 0x08; //CS
		PUSH entryPoint; //EIP
		IRETD; //위의 값들의 설정으로 인해 SystemConsoleProc루틴으로 전환된다. _인터럽트 복귀 명렁어 
	}
}

DWORD WINAPI SystemConsoleProc(LPVOID parameter){//콘솔 프로세스 
	SkyConsole::Print("Console Mode Start!!\n");
	
	KeyBoardController::SetupInterrupts();//콘솔용 키보드 초기화 
	
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);//타이머 활성화 
	
	multiboot_info* pBootInfo=SkyModuleManager::GetInstance()->GetMultiBootInfo();//저장장치정보를 가져와서 초기화 
	StorageManager::GetInstance()->Initialize(pBootInfo);
	
	NativeConsole();
	
	SkyConsole::Print("Bye!!");
	return 0;
}

void NativeConsole(){
	ConsoleManager manager;
	
	char commandBuffer[MAXPATH];
	while(true){
		SkyConsole::Print("Command> ");
		memset(commandBuffer, 0, MAXPATH);
		
		SkyConsole::GetCommand(commandBuffer, MAXPATH-2);
		SkyConsole::Print("\n");
		
		if(manager.RunCommand(commandBuffer)==true)//커맨드가 정상적으로 실행된다면 콘솔을 종료 
			break;
	}
}

//WATCHDOG
#define TS_WATCHDOG_CLOCK_POS	(0xb800+(80-1)*2)

DWORD WINAPI WatchDogProc(LPVOID parameter){//콘솔에서 시스템이 정상적으로 멀티태스킹이 되고있는지를 알기 위하여 우측상단에 바람개비형태로 일정시간마다 문자를 바꾼다. 
	Process* pProcess=(Process*)parameter;
	int pos=0;
	char *addr=(char*)TS_WATCHDOG_CLOCK_POS, status[]={ '-', '\\', '|', '/', '-', '\\', '|', '/'};//어디에 문자를 저장할건지, 문자모양들 
	int first=GetTickCount();
	
	while(true){
		int second=GetTickCount();
		if(second-first>=TIMEOUT_PER_SECOND){
			if(++pos>7)
				pos=0;
			
			if(m_bShowTSWatchdogClock)
				*addr=status[pos];//변경할 문자 
			
			first=GetTickCount();
		}
	}
	return 0;
}

//ProcessRemoverProc
DWORD WINAPI ProcessRemoverProc(LPVOID parameter){
	Process* pProcess=(Process*)parameter;
	
	int static id=0;
	int temp=id++;
	int first=GetTickCount();
	
	while(true){
		kEnterCriticalSection();
		
		ProcessManager::GetInstance()->RemoveTerminatedProcess();//종룓된 프로세스들을 제거하고 
		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());//현재 프로세스의 아이디에 메모리를 양도한다.? 
		
		kLeaveCriticalSection();
	}
	return 0;
}

//콘솔 프레임워크 시스템: 콘솔 명령을 쉽게 추가하고 관리할 수 있게하는 툴
long CmdCls(char *theCommand);
long CmdKill(char *theCommand);
long CmdProcessList(char *theCommand);
long cmdMemState(char *theCommand); 
long cmdCreateWatchdogTask(char *theCommand);
long cmdTaskCount(char *theCommand);
long cmdGlobalState(char *theCommand);
long CmdExec(char *theCommand);
long cmdPCI(char *theCommand);
long cmdDir(char *theCommand);

START_COMMAND_TABLE
	
	CMD_ENTRY("q", false, NULL, "quits and halts the system")//명령어 키워드, 매개변수가 있는지, 실행될 함수, 설명 
	CMD_ENTRY("help", false, NULL, "Help me please")
	CMD_ENTRY("cls", false, clsCmd, "Clear screen")
	CMD_ENTRY("test", false, cmdTestCPlusPlus, "Test C++ Class")
	CMD_ENTRY("memstate", false, cmdMemState, "Print memory state")
	CMD_ENTRY("process", false, cmdProcessList, "Print process list")
	CMD_ENTRY("kill", true, cmdKillTask, "Kill process with process id")
	CMD_ENTRY("read", true, cmdRead, "reads a file")
	CMD_ENTRY("gui", false, cmdGUI, "Jump to graphic user interface");

END_COMMAND_TABLE	
