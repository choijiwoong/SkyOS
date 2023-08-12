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

//SkyGUIConsoleŬ���� ����
class SkyGUIConsole{
	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);
	.....
}; 

class SkyGUI{//�Ȱ��� �޼ҵ常 ���Ͻ�Ű�� �ڵ��浹���� ���ο� GUI ���� ���� 
	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run();
	bool Print(char* pMsg);
};

//�׷��� ��� Ŭ������ ��� ���ø� Ŭ����(�������̽�)
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

//���ø� Ŭ������ �̿��� ����_�̸� ��ӹ޴� Ŭ������ Print�� Run���� �޼ҵ带 �����ؾ߸� �������� �ȴ�. 
SkyWindow<SKY_GUI_SYSTEM>* m_pWindow; 
