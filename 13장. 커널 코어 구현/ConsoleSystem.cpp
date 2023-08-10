//SkyOS�� �ֽܼý���: �����Է��� �޴� �ܼ� �Ŵ���, ����� ���μ����� �����ϴ� ���μ��� ������, ��Ƽ�½�ŷ�����ϴ� WatchDog

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack);
DWORD WINAPI SystemConsoleProc(LPVOID parameter);
void NativeConsole();
DWORD WINAPI WatchDogProc(LPVOID parameter);
DWORD WINAPI ProcessRemoverProc(LPVOID parameter); 

void StartConsoleSystem(){
	kEnterCriticalSection();
	
	//�⺻ ���μ��� ���� 
	Process* pProcess=ProcessManager::GetInstance()->CreateProcessFromMemory("ConsoleSystem", SystemConsoleProc, NULL, PROCESS_KERNEL);
	ProcessManager::GetInstance()->CreateProcessFromMemory("WatchDog", WatchDogProc, NULL, PROCESS_KERNEL);
	ProcessManager::GetInstance()->CreateProcessFromMemory("ProcessRemover", ProcessRemoverProc, NULL, PROCESS_KERNEL);
	if(pProcess==nullptr)
		HaltSystem("Console Creation Fail!!");
	
	SkyConsole::Print("Init Console....\n");
	
	//�ܼ����μ����� ���� �����带 ������ 
	Thread* pThread=pProcess->GetMainThread();
	if(pThread==nullptr)
		HaltSystem("Console Creation Fail!!");
 
	pThread->m_taskState=TASK_STATE_RUNNING;//�����ǥ�� 
	
	//��Ʈ�����ν��� ������ �����ϰ� 
	int entryPoint=(int)pThread->frame.eip;
	unsigned int procStack=pThread->frame.esp;
	
	kLeaveCriticalSection();
	
	//�����Ѵ�_�ֽܼý������� 
	JumpToNewKernelEntry(entryPoint, procStack);
}

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack){
	__asm{
		MOV AX, 0x10; //GDT���� 0x10���� Ŀ�� ������ ��ũ�����̱⿡ ���׸�Ʈ ������ �������͸� �ʱ�ȭ. 
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
		IRETD; //���� ������ �������� ���� SystemConsoleProc��ƾ���� ��ȯ�ȴ�. _���ͷ�Ʈ ���� ���� 
	}
}

DWORD WINAPI SystemConsoleProc(LPVOID parameter){//�ܼ� ���μ��� 
	SkyConsole::Print("Console Mode Start!!\n");
	
	KeyBoardController::SetupInterrupts();//�ֿܼ� Ű���� �ʱ�ȭ 
	
	StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);//Ÿ�̸� Ȱ��ȭ 
	
	multiboot_info* pBootInfo=SkyModuleManager::GetInstance()->GetMultiBootInfo();//������ġ������ �����ͼ� �ʱ�ȭ 
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
		
		if(manager.RunCommand(commandBuffer)==true)//Ŀ�ǵ尡 ���������� ����ȴٸ� �ܼ��� ���� 
			break;
	}
}

//WATCHDOG
#define TS_WATCHDOG_CLOCK_POS	(0xb800+(80-1)*2)

DWORD WINAPI WatchDogProc(LPVOID parameter){//�ֿܼ��� �ý����� ���������� ��Ƽ�½�ŷ�� �ǰ��ִ����� �˱� ���Ͽ� ������ܿ� �ٶ��������·� �����ð����� ���ڸ� �ٲ۴�. 
	Process* pProcess=(Process*)parameter;
	int pos=0;
	char *addr=(char*)TS_WATCHDOG_CLOCK_POS, status[]={ '-', '\\', '|', '/', '-', '\\', '|', '/'};//��� ���ڸ� �����Ұ���, ���ڸ��� 
	int first=GetTickCount();
	
	while(true){
		int second=GetTickCount();
		if(second-first>=TIMEOUT_PER_SECOND){
			if(++pos>7)
				pos=0;
			
			if(m_bShowTSWatchdogClock)
				*addr=status[pos];//������ ���� 
			
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
		
		ProcessManager::GetInstance()->RemoveTerminatedProcess();//������ ���μ������� �����ϰ� 
		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());//���� ���μ����� ���̵� �޸𸮸� �絵�Ѵ�.? 
		
		kLeaveCriticalSection();
	}
	return 0;
}

//�ܼ� �����ӿ�ũ �ý���: �ܼ� ����� ���� �߰��ϰ� ������ �� �ְ��ϴ� ��
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
	
	CMD_ENTRY("q", false, NULL, "quits and halts the system")//��ɾ� Ű����, �Ű������� �ִ���, ����� �Լ�, ���� 
	CMD_ENTRY("help", false, NULL, "Help me please")
	CMD_ENTRY("cls", false, clsCmd, "Clear screen")
	CMD_ENTRY("test", false, cmdTestCPlusPlus, "Test C++ Class")
	CMD_ENTRY("memstate", false, cmdMemState, "Print memory state")
	CMD_ENTRY("process", false, cmdProcessList, "Print process list")
	CMD_ENTRY("kill", true, cmdKillTask, "Kill process with process id")
	CMD_ENTRY("read", true, cmdRead, "reads a file")
	CMD_ENTRY("gui", false, cmdGUI, "Jump to graphic user interface");

END_COMMAND_TABLE	
