//GRUB 1.X버전이 커널로 제어권 이양 시 그래픽 관련 정보를 제대로 넘겨주지 않는 반면, 2.X부터 제대로 정보가 넘어온다. 멀티부트구조체역시 일부 수정 
_declspec(naked) void multiboot_entry(void){
	.....
	//멀티부트 헤더 사이즈: 0x30
	dd(MULTIBOOT_HEADER_MAGIC); magic number
	
	#if SKY_CONSOLE_MODE == 0 //0이면 그래픽모드, 1이면 콘솔모드 
		dd(MULTIBOOT_HEADER_FLAGS_GUI);//GUI 플래그
		dd(CHECKSUM_GUI); //GUI 체크썸
	#else
		dd(MULTIBOOT_HEADER_FLAGS);
		dd(CHECKSUM);
	#endif
		dd(HEADER_ADDRESS);
		dd(KERNEL_LOAD_ADDRESS);
		dd(00);
		dd(00);
		dd(HEADER_ADDRESS+0x30);
		//커널시작주소: 멀티부트헤더주소+0x30, kernel_entry 
		dd(SKY_CONSOLE_MODE);//0일 시 그래픽모드로의 부팅을 준비. 만약 1이면 WIDTH, HEIGHT, BPP와 상관없이 콘솔모드로 부팅 
		dd(SKY_WIDTH);
		dd(SKY_HEIGHT);
		dd(SKY_BPP);
		.....
}

//SkyConsoleTask시작_SkyGUI가 전달한 메시지를 리스너가 폴링하다가 데이터를 받으면 처리하는데 이 작업을 수행 
bool SkyConsoleTask::Run(){
	SKY_MSG msg;
	while(kGetMessage(&msg, nullptr, 0, 0)){
		if(!kTranslateAccelerator(msg._hwnd, nullptr, &msg)){
			kTranslateMessage(&msg);
			kDispatchMessage(&msg);
		}
	}
	return msg._msgId=SKY_MSG_EXIT;
}

bool SkyConsoleTask::kGetMessage(LPSKY_MSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax){
	.....
	if(m_pIOListener->ReadyStatus()==false){
		Scheduler::GetInstance()->Yield(m_ProcessId);
	} else{
		lpMsg->_msgId=SKY_MSG_MESSAGE;
		lpMsg->_extra=m_pIOListener->GetStatus();
	}
	.....
}

void SkyConsoleTask::ProcessKeyboard(int value){
	if(IsBackSpace(value)){//벡스페이스라면 
		if(m_cursorX>16){ //지울 수 있다면 
			m_pRenderer->PutFontAscToSheet(m_pSheet, m_cursorX, m_cursorY, COL8_FFFFFF, COL8_000000, " ", 1);//공백으로 바꾸고 
			m_cursorC-=8;//커서 이동 
		}
	} else if(IsEnter(value)){//엔터라면(뭔갈 치고 엔터) 
		m_pRenderer->PutFontAscToSheet(m_pSheet, m_cursorX, m_cursorY, COL8_FFFFFF, COL8_000000, " ", 1);//공백으로 바꾸고 
		cmdline[m_cursorX/8-2]=0;//cmd라인을 초기화하고 
		GetNewLine();//줄바꿈 
		
		if(strcmp(cmdline, "mem")==0){
			size_t totalMemory=PhysicalMemoryManager::GetMemorySize();
			sprintf(s, "total %dMB", totalMemory/(1024*1024));
			m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8_000000, s, 30);//현재 시트에 s값 출력 
			GetNewLine();
			sprintf(s, "free %dKB", PhysicalMemoryManager::GetFreeMemory()/1024);
			m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8_000000, s, 30);
			GetNewLine();
		} else if(strcmp(cmdline, "cls")==0){
			unsigned char* buf=m_pSheet->GetBuf();//버퍼를 가져오고 
			int bxsize=m_pSheet->GetXSize();//사이즈를 가져와 
			for(int y=28; y<28; y++)//전체 화면을 
				for(int x=8; x<8+240; x++)
					buf[x+y+bxsize]=COL8_000000;//흰색으로 초기화 
			m_pSheet->Refresh(8, 28, 8+240, 28+128);//새로고침을 하고 
			m_cursorY=28;//커서를 이동 
		} else if(cmdline[0]!=0){//이상한 커맨드 
			m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8000000, "Bad command.", 12);
			GetNewLine();
		}
		
		m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8_000000, ">", 1);//다음 명령어 입력을 위함 
		m_cursorX+=16;
	} else{//명령어를 입력중이라면 
		m_pRenderer->PutFontAscToSheet(m_pSheet, m_cursorX, m_cursorY, COL8_FFFFFF, COL8_000000, s, 1);//출력하고 커서이동만 
		m_cursorX+=8;
	}
	
	if(m_cursorCol>=0)//커서를 표시 
		m_pRenderer->BoxFill(m_pSheet->GetBuf(), m_pSheet->GetXSize(), m_cursorCol, m_cursorX, m_cursorY, m_cursoeX+7, m_cursorY+15);
	m_pSheet->Refresh(m_cursorX, m_cursorY, m_cursorX+8, m_cursorY+16);//시트를 갱신 
}

////SkyWindow클래스 시작(인터페이스)
template <typename T>
class SkyWindow{
	public:
		SkyWindow()=default;
		virtual ~SkyWindow()=default;
		
		bool Initialize(void* _pVideoRamPtr, int _width, int _bpp, uint8_t buffertype);
		bool Run();
		bool Clear();
		bool Print(char* pMsg);
		
		T m_guiSys;
}; 

template <typename T>
bool SkyWindow<T>::Initialize(void* _pVideoRamPtr, int _width, int _bpp, uint8_t buffertype){
	return m_guiSys.Initialize(_pVideoPtr, _width, _height, _bpp, buffertype);
}

template <typename T>
bool SkyWindow<T>::Run(){
	return m_guiSys.Run();
}

template <typename T>
bool SkyWindow<T> Print(char* pMsg){
	return m_guiSys.print(pMsg);
}

template <typename T>
bool SkyWindow<T>::Clear(){
	return m_guiSys.clear();
}

SkyWindow<SKY_GUI_SYSTEM>* m_pWindow; //템플릿 클래스를 이용한 예시_이를 상속받는 클래스는 Print나 Run같은 메소드를 구현해야만 컴파일이 된다. 


//SkyGUI클래스 시작
class SkyGUI{//똑같이 메소드만 통일시키면 코드충돌없이 새로운 GUI 개발 가능 
	.....
	void RegisterIOOListener(int processID, ConsoleIOListener* listener);
	SkyRenderer* GetRenderer(){ return m_pRenderer; }
	SkySheet* FindSheetById(int processId);
	void FillRect8(int x, int y, int w, int h, char col, int actualX, int actualY);
	static bool LoadFontFromMemory();
	 
	protected:
		bool MakeInitScreen();
		bool MakeIOSystem();
		
		void ProcessKeyboard(int value);
		void ProcessMouse(int value);
		
		bool kGetMessage(LPSKY_MSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
		bool kTranslateAccelerator(HWND hWnd, HANDLE hAccTable, LPSKY_MSG lpMsg);
		bool kTranslateMessage(const LPSKY_MSG lpMsg);
		bool kDispatchMessage(const LPSKY_MSG lpMsg);
		bool CALLBACK kWndProc(HWND hWnd, UINT message, WPARAM, wParam, LPATAM lParam);
		
		void ProcessMouseLButton(int x, int y);
		void CreateGUIConsoleProcess(int xPos, int yPos);
		bool CreateGUIDebugProcess();
		bool SendToMessage(SkySheet* pSheet, int message, int value);
		bool SendToMessage(int processId, char* pMsg);
		
		map<int, ConsoleIOListener*> m_mapIOListener;
		.....
	 
	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);
	
	
};

static bool SkyGUI::LoadFontFromMemory(){
	unsigned char* buffer=(unsigned char*)skyFontData;
	int bufferIndex=0;
	int charIndex=0;
	memset(buffer, 0, 4096);
	int readIndex=0;
	
	while(readIndex<32768){
		for(int i=0; i<128; i++){
			if(fontData[readIndex+i]=='*'){
				if(charIndex<8){
					char a=(char)(1<<(8-1-charIndex));//.은 빈픽셀, *이 출력될 픽셀_직접 읽는건가..? 
					buffer[bufferIndex]|=a;//버퍼에 바이트단위 or연산
				}
			}
			
			if((fontData[readIndex+i]=='*') || (fontData[readIndex+i]=='.')){
				charIndex++;
				if(charIndex>=8){
					bufferIndex++;
					charIndex=0;
				}
			}
		}
		readIndex+=128;
	}
	return true;
}

bool SkyGUI::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype){
	m_pVideoRamPtr=(ULONG*)pVideoRamPtr;
	m_width=width;
	m_heightr=height;
	m_bpp=bpp;
	
	LoadFontFromMemory();
	MakeInitScreen();//초기 화면 구성
	
	return true; 
}

bool SkyGUI::Run(){
	MakeIOSystem();//IO구축: GUI전용 키보드, 마우스 핸들러 설정. FIFO를 사용해 각 프로세스, 컴포넌트가 마우스와 키보드 이벤트를 받을 수 있도록 준비 
	
	CreateGUIDebugProcess();//디버그콘솔창 
	CreateGUIConsoleProcess(300, 4);//일반콘솔창
	
	SKY_MSG msg;
	while(kGetMessage(&msg, nullptr, 0, 0)) {//가져온 메시지가 SKY_MSG_EXIT(false)일 경우에만 escape 
		if(!kTranslateAccelerator(msg._hwnd, nullptr, &msg)){
			kTranslateMessage(&msg);
			kDispatchMessage(&msg);
		}
	}
	return msg._msgId==SKY_MSG_EXIT;
}


DWORD WINAPI ConsoleGUIProc(LPVOID parameter){
	kEnterCriticalSection();
	Thread* pThread=ProcessManager::GetInstance()->GetCurrentTask();
	SkyGUI* pGUI=(SkyGUI*)parameter;
	SkyConsoleTask* pTask=new SkyConsoleTask();//태스크 만들고 
	pTask->Init(pGUI, pThread);//SkyGUI와 스레드를 이용해 스레드 초기화 
	kLeaveCriticalSection();
	
	pTask->Run();
	return 0;
}
void SkyGUI::CreateGUIConsoleProcess(int xPos, int yPos){
	Process* pProcess=nullptr;
	pProcess=ProcessManager::GetInstance()->CreateProcessFromMemory("GUIConsole", ConsoleGUIProc, this, PROCESS_KERNEL);
	
	if(pProcess!=nullptr){
		//콘솔 태스크 시트를 구성?
		SkySheet* console=m_mainSheet->Alloc();
		unsigned char* buf=m_mainSheet->AllocBuffer(256, 165);
		console->SetBuf(buf, 256, 165, -1);//콘솔에 할당받은 버퍼를 할당하는 과정인듯 
		
		//타이틀바
		m_pRenderer->MakeWindow(buf, 256, 165, "Sky Console", 0);
		m_pRenderer->MakeTextBox(console, 8, 28, 240, 128, COL8_000000);
		
		//콘솔창의 위치를 이동
		console->Slider(xPos, yPos);
		
		//윈도우 출력 우선순위 지정
		console->Updown(20);
		console->m_ownerProcess=pProcess->GetProcessId(); 
	}
}

void ProcessSkyKeyboardHandler(){//키보드 이벤트를 받으면 PIC에 알리고 데이터르 받는다 
	int data;
	OutPortByte(PIC0_OCW2, 0x61);//IRQ-01접수 완료를 PIC에 통지.....
	data=InPortByte(PORT_KEYDAT);//데이터 읽기 
	fifo32_put(keyfifo, data+keyboard0);//데이터를 FIFO 자료구조에 저장 
	return;
}
__declspec(naked) void kSkyKeyboardHandler(){
	.....
	_asm{
		call ProcessSkyKeyboardHandler;
	}
	.....
}
bool SkyGUI::KGetMessage(LPSKY_MSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax){
	.....
	if(fifo32_status(&fifo)!=0){//FIFO에 데이터가 있다면 
		lpMsg->_msgId=SKY_MSG_MESSSAGE;//메시지 생성 
		lpMsg->_extra=fifo32_get(&fifo);
	}
	.....
}
bool CALLBACK SkyGUI::kWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
		case SKY_MSG_KEYBOARD:
			{
				ProcessKeyboard(wParam);//키보드 이벤트를 받으면 
			}
			break;
		
		case SKY_MSG_MOUSE:
			{
				ProcessMouse(wParam);
			}
			break;
		
		case SKY_MSG_EXIT:
			break;
			
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool SkyGUI::SendToMessage(SkySheet* pSheet, int message, int value){//키보드 이벤트를 받아 ProcessKeyboard메서드가 실행되는데, 이 때 데이터를 아스키로 변경하고 메시지를 현재 쉬트에 보낸다. 
	.....
	map<int, ConsoleIOListener*>::iterator iter;
	iter=m_mapIOListener.find(pSheet->m_ownerProcess);//현재 쉬트를 소유중인 프로세서의 IO리스터를 찾아서 
	if(iter==m_mapIOListener.end())
		return false;
	
	ConsoleIOListener* listener=(*iter).second;
	listener->PushMessage(message, value);//데이터와 함께 메시지를 보낸다 
	
	return true;
}

