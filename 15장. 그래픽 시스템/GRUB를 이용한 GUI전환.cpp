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

//SkyGUIConsole클래스 시작
class SkyGUIConsole{
	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);
	.....
}; 

class SkyGUI{//똑같이 메소드만 통일시키면 코드충돌없이 새로운 GUI 개발 가능 
	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);
};

//그래픽 모드 클래스를 담는 템플릿 클래스(인터페이스)
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

//템플릿 클래스를 이용한 예시_이를 상속받는 클래스는 Print나 Run같은 메소드를 구현해야만 컴파일이 된다. 
SkyWindow<SKY_GUI_SYSTEM>* m_pWindow; 
