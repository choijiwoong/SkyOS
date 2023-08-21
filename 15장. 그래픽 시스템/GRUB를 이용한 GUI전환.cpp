//GRUB 1.X������ Ŀ�η� ����� �̾� �� �׷��� ���� ������ ����� �Ѱ����� �ʴ� �ݸ�, 2.X���� ����� ������ �Ѿ�´�. ��Ƽ��Ʈ����ü���� �Ϻ� ���� 
_declspec(naked) void multiboot_entry(void){
	.....
	//��Ƽ��Ʈ ��� ������: 0x30
	dd(MULTIBOOT_HEADER_MAGIC); magic number
	
	#if SKY_CONSOLE_MODE == 0 //0�̸� �׷��ȸ��, 1�̸� �ָܼ�� 
		dd(MULTIBOOT_HEADER_FLAGS_GUI);//GUI �÷���
		dd(CHECKSUM_GUI); //GUI üũ��
	#else
		dd(MULTIBOOT_HEADER_FLAGS);
		dd(CHECKSUM);
	#endif
		dd(HEADER_ADDRESS);
		dd(KERNEL_LOAD_ADDRESS);
		dd(00);
		dd(00);
		dd(HEADER_ADDRESS+0x30);
		//Ŀ�ν����ּ�: ��Ƽ��Ʈ����ּ�+0x30, kernel_entry 
		dd(SKY_CONSOLE_MODE);//0�� �� �׷��ȸ����� ������ �غ�. ���� 1�̸� WIDTH, HEIGHT, BPP�� ������� �ָܼ��� ���� 
		dd(SKY_WIDTH);
		dd(SKY_HEIGHT);
		dd(SKY_BPP);
		.....
}

//SkyConsoleTask����_SkyGUI�� ������ �޽����� �����ʰ� �����ϴٰ� �����͸� ������ ó���ϴµ� �� �۾��� ���� 
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
	if(IsBackSpace(value)){//�������̽���� 
		if(m_cursorX>16){ //���� �� �ִٸ� 
			m_pRenderer->PutFontAscToSheet(m_pSheet, m_cursorX, m_cursorY, COL8_FFFFFF, COL8_000000, " ", 1);//�������� �ٲٰ� 
			m_cursorC-=8;//Ŀ�� �̵� 
		}
	} else if(IsEnter(value)){//���Ͷ��(���� ġ�� ����) 
		m_pRenderer->PutFontAscToSheet(m_pSheet, m_cursorX, m_cursorY, COL8_FFFFFF, COL8_000000, " ", 1);//�������� �ٲٰ� 
		cmdline[m_cursorX/8-2]=0;//cmd������ �ʱ�ȭ�ϰ� 
		GetNewLine();//�ٹٲ� 
		
		if(strcmp(cmdline, "mem")==0){
			size_t totalMemory=PhysicalMemoryManager::GetMemorySize();
			sprintf(s, "total %dMB", totalMemory/(1024*1024));
			m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8_000000, s, 30);//���� ��Ʈ�� s�� ��� 
			GetNewLine();
			sprintf(s, "free %dKB", PhysicalMemoryManager::GetFreeMemory()/1024);
			m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8_000000, s, 30);
			GetNewLine();
		} else if(strcmp(cmdline, "cls")==0){
			unsigned char* buf=m_pSheet->GetBuf();//���۸� �������� 
			int bxsize=m_pSheet->GetXSize();//����� ������ 
			for(int y=28; y<28; y++)//��ü ȭ���� 
				for(int x=8; x<8+240; x++)
					buf[x+y+bxsize]=COL8_000000;//������� �ʱ�ȭ 
			m_pSheet->Refresh(8, 28, 8+240, 28+128);//���ΰ�ħ�� �ϰ� 
			m_cursorY=28;//Ŀ���� �̵� 
		} else if(cmdline[0]!=0){//�̻��� Ŀ�ǵ� 
			m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8000000, "Bad command.", 12);
			GetNewLine();
		}
		
		m_pRenderer->PutFontAscToSheet(m_pSheet, 8, m_cursorY, COL8_FFFFFF, COL8_000000, ">", 1);//���� ��ɾ� �Է��� ���� 
		m_cursorX+=16;
	} else{//��ɾ �Է����̶�� 
		m_pRenderer->PutFontAscToSheet(m_pSheet, m_cursorX, m_cursorY, COL8_FFFFFF, COL8_000000, s, 1);//����ϰ� Ŀ���̵��� 
		m_cursorX+=8;
	}
	
	if(m_cursorCol>=0)//Ŀ���� ǥ�� 
		m_pRenderer->BoxFill(m_pSheet->GetBuf(), m_pSheet->GetXSize(), m_cursorCol, m_cursorX, m_cursorY, m_cursoeX+7, m_cursorY+15);
	m_pSheet->Refresh(m_cursorX, m_cursorY, m_cursorX+8, m_cursorY+16);//��Ʈ�� ���� 
}

////SkyWindowŬ���� ����(�������̽�)
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

SkyWindow<SKY_GUI_SYSTEM>* m_pWindow; //���ø� Ŭ������ �̿��� ����_�̸� ��ӹ޴� Ŭ������ Print�� Run���� �޼ҵ带 �����ؾ߸� �������� �ȴ�. 


//SkyGUIŬ���� ����
class SkyGUI{//�Ȱ��� �޼ҵ常 ���Ͻ�Ű�� �ڵ��浹���� ���ο� GUI ���� ���� 
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
					char a=(char)(1<<(8-1-charIndex));//.�� ���ȼ�, *�� ��µ� �ȼ�_���� �д°ǰ�..? 
					buffer[bufferIndex]|=a;//���ۿ� ����Ʈ���� or����
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
	MakeInitScreen();//�ʱ� ȭ�� ����
	
	return true; 
}

bool SkyGUI::Run(){
	MakeIOSystem();//IO����: GUI���� Ű����, ���콺 �ڵ鷯 ����. FIFO�� ����� �� ���μ���, ������Ʈ�� ���콺�� Ű���� �̺�Ʈ�� ���� �� �ֵ��� �غ� 
	
	CreateGUIDebugProcess();//������ܼ�â 
	CreateGUIConsoleProcess(300, 4);//�Ϲ��ܼ�â
	
	SKY_MSG msg;
	while(kGetMessage(&msg, nullptr, 0, 0)) {//������ �޽����� SKY_MSG_EXIT(false)�� ��쿡�� escape 
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
	SkyConsoleTask* pTask=new SkyConsoleTask();//�½�ũ ����� 
	pTask->Init(pGUI, pThread);//SkyGUI�� �����带 �̿��� ������ �ʱ�ȭ 
	kLeaveCriticalSection();
	
	pTask->Run();
	return 0;
}
void SkyGUI::CreateGUIConsoleProcess(int xPos, int yPos){
	Process* pProcess=nullptr;
	pProcess=ProcessManager::GetInstance()->CreateProcessFromMemory("GUIConsole", ConsoleGUIProc, this, PROCESS_KERNEL);
	
	if(pProcess!=nullptr){
		//�ܼ� �½�ũ ��Ʈ�� ����?
		SkySheet* console=m_mainSheet->Alloc();
		unsigned char* buf=m_mainSheet->AllocBuffer(256, 165);
		console->SetBuf(buf, 256, 165, -1);//�ֿܼ� �Ҵ���� ���۸� �Ҵ��ϴ� �����ε� 
		
		//Ÿ��Ʋ��
		m_pRenderer->MakeWindow(buf, 256, 165, "Sky Console", 0);
		m_pRenderer->MakeTextBox(console, 8, 28, 240, 128, COL8_000000);
		
		//�ܼ�â�� ��ġ�� �̵�
		console->Slider(xPos, yPos);
		
		//������ ��� �켱���� ����
		console->Updown(20);
		console->m_ownerProcess=pProcess->GetProcessId(); 
	}
}

void ProcessSkyKeyboardHandler(){//Ű���� �̺�Ʈ�� ������ PIC�� �˸��� �����͸� �޴´� 
	int data;
	OutPortByte(PIC0_OCW2, 0x61);//IRQ-01���� �ϷḦ PIC�� ����.....
	data=InPortByte(PORT_KEYDAT);//������ �б� 
	fifo32_put(keyfifo, data+keyboard0);//�����͸� FIFO �ڷᱸ���� ���� 
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
	if(fifo32_status(&fifo)!=0){//FIFO�� �����Ͱ� �ִٸ� 
		lpMsg->_msgId=SKY_MSG_MESSSAGE;//�޽��� ���� 
		lpMsg->_extra=fifo32_get(&fifo);
	}
	.....
}
bool CALLBACK SkyGUI::kWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message){
		case SKY_MSG_KEYBOARD:
			{
				ProcessKeyboard(wParam);//Ű���� �̺�Ʈ�� ������ 
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

bool SkyGUI::SendToMessage(SkySheet* pSheet, int message, int value){//Ű���� �̺�Ʈ�� �޾� ProcessKeyboard�޼��尡 ����Ǵµ�, �� �� �����͸� �ƽ�Ű�� �����ϰ� �޽����� ���� ��Ʈ�� ������. 
	.....
	map<int, ConsoleIOListener*>::iterator iter;
	iter=m_mapIOListener.find(pSheet->m_ownerProcess);//���� ��Ʈ�� �������� ���μ����� IO�����͸� ã�Ƽ� 
	if(iter==m_mapIOListener.end())
		return false;
	
	ConsoleIOListener* listener=(*iter).second;
	listener->PushMessage(message, value);//�����Ϳ� �Բ� �޽����� ������ 
	
	return true;
}

