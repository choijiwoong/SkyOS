#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
//GRUB�� �䱸�ϴ� �ñ״�ó(����Ʈ ��� ����ü�� ��) 
_declspec(naked) void multiboot_entry(void){
	__asm{//dd�� define double word�� �ű׷η� 4����Ʈ�� ���� Ȯ�� 
		align 4//4����Ʈ ���� 
		multiboot_header:
			dd(MULTIBOOT_HEADER_MAGIC); magic number
			dd(MULTIBOOT_HEADER_FLAGS); flags
			dd(CHECKSUM); checksum
			dd(HEADER_ADRESS); //����ּ� KERNEL_LOAD_ADDRESS+ALIGN (0x100400).
			//��, KERNEL_LOAD_ADDRESS�� 0x100000�����ε� AliGN�� 4����Ʈ�� ������ �ε�� �ּ� ������ KERNEL_HEADER_ADDRESS�� �ȴ�. 0x100400����. 
			dd(KERNEL_LOAD_ADDRESS);//Ŀ���� �θ��� �ּ�
			dd(00);
			dd(00);
			dd(HEADER_ADRESS+0x20);//Ŀ�� ���� �ּ�. kernel_entry. 
		
		kernel_entry:
			mov esp, KERNEL_STACK;//���� ���� 
			
			push 0;//�÷��� ���������� �ʱ�ȭ 
			popf
			
			//GRUB�� �������� Ǫ�� 
			push ebx;//Multiboot_Info����ü�� ������ 
			push eax;//�����ѹ� 
			
			call kmain;//���� �� �Ķ���͸� ������� kmain�Լ��� ȣ���Ѵ�. 
			
			halt://kmain�� ���ϵ� ������ ����x 
				jmp halt;
	}
} 

//MULTIBOOT_HEADER����ü(���� ���� ����)
struct MULTIBOOT_HEADER{
	uint32_t magic;
	uint32_t flags;
	uint32_t checksum;
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;//header_addr+0x20
}; 

void kmain(unsigned long magic, unsigned long addr){//�����ѹ�, Multiboot_info����ü�� ������
	InitializeConstructor();//�۷ι� ��ü �ʱ�ȭ
	SkyConsole::Initialize();//ȭ�鿡 ���ڸ� ������� �ʱ�ȭ
	SkyConsole::Print("Hello world!\n") ;
	for(;;);//�����Լ��� ������ ���¿뵵.. 
}

//***************kmain����*******************
namespace SkyConcole{//�ܼ� Ŭ���� �������̽�(80*25 �ַܼΰ�. �̱��� ��ü �������� ����ϱ� ���� ���ӽ����̽� ���) 
	void Initialize();
	void Clear();
	void WriteChar(char c, ConsoleColor textColour, ConsoleColor backColour);
	void WriteString(const char* szString, Console textColour=White, ConsoleColor backColour=Black);
	void Write(const char *szString);
	void WriteChar(char c);
	
	void Print(const char* str, ...);
	
	void MoveCursor(unsigned int X, unsigned int Y);
	void GetCursorPos(uint& x, uint& y);
	void setCursorType(unsigned char Bottom, unsigned char Top);
	void scrollup();
	
	void SetColor(ConsoleColor Text, ConsoleColor Back, bool blink);
	unsigned char GetBackColor();
	unsigned char GetTextColor();
	void SetBackColor(ConsoleColor col);
	void SetTextColor(ConsoleColor col);
}

void SkyConcole::Initialize(){
	char c=(*(unsigned short*)0x410 & 0x30);
	if(c==0x30){//���� ī�尡 ����̶�� 
		m_pVideoMemory=(unsigned short*)0xb0000;
		m_VideoCardType=VGA_MONO_CRT_ADDRESS;//����ī��Ÿ���� ������� ���� 
	} else{//���� ī�尡 �÷���� 
		m_pVideoMemory=(unsigned short*)0xb8000;
		m_VideoCardType=VGA_COLOR_CRT_ADDRESS;//����ī��Ÿ���� �÷��� ���� 
	}
	
	//ȭ������� 80x25 
	m_ScreenHeight=25;
	m_ScreenWidth=80;
	
	//Ŀ�� ��ġ �ʱ�ȭ 
	m_xPos=0;
	m_yPos=0;
	
	//������ 
	m_Text=White;
	m_backGroundColor=Black;
	m_Color=(ConsoleColor)((m_backGroundBolor<<4)|m_text);//���� 4��Ʈ�� ����, �ڿ� 4��Ʈ�� �ؽ�Ʈ ����
	
	Clear(); 
}

void SkyConsole::Print(const char* str, ...){
	va_list args;
	va_start(args, str);//args�� ù��° �μ��� ����Ű���� �ʱ�ȭ 
	for(size_t i=0; i<strlen(str); i++){
		switch(str[i]){
			case '%':
				switch(str[i+1]){
					case 'c': {//���� 
						char c=va_arg(args, char);//���ڸ� ������ 
						WriteChar(c, m_Text, m_backGroundColor);
						i++;
						break;
					}
						
					case 's': {//���ڿ�
						int c=(int&)va_arg(args, char);//�ƽ�Ű�ڵ� 
						char str[256];
						strcpy(str, (const char*)c);
						Write(str);
						i++;
						break;
					}
					
					case 'd':
					case 'i': {//���� 
						int c=va_arg(args, int);
						char str[32]={0};
						itoa_s(c, 10, str);//char* "1000"�� int 1000���� 
						Write(str);
						i++;
						break;
					}
					
					case 'x': {//16���� 
						unsigned int c=va_arg(args, unsigned int);
						char str[32]={0};
						itoa_s(c, 16, str);
						Write(str);
						i++;
						break;
					}
					
					case 'f': {//4���� FPU init ���� �߰�
						double double_temp;
						double_temp=va_arg(args, double);
						char buffer[512];
						ftoa_fixed(buffer, double_temp);//double�� char*�� �ٲ� 
						Write(buffer);
						i++;
						break;
					}
						
					
					default://%tó�� �̻��Ѱ� ������ �������� 
						va_end(args);
						return;
					}
				break;
			
			default://%d������ �ƴ� �Ϲݹ��ڴ� �׳� ��� 
				WriteChar(str[i], m_Text, m_backGroundColor);
				break;
		}
	} 
	va_end(args);
}

void SkyConsole::WriteChar(char c, ConsoleColor textColor, ConsoleColor backColor){
	int t;
	switch(c){
		case '\n'://�ٹٲ��̸� 
			m_xPos=0;
			m_yPos++;//���콺 �������̵� 
			break;
			
		case 8://�齺���̽���
			t=m_xPos+m_yPos*m_ScreenWidth;//���� ���� Ŀ���� ��ġ�� ���� 
			if(t>0)//�Ϲ����� ����� 
				t--;//������ �̵�(����� ������) 
			
			if(m_xPos>0){//�� ������ �ƴϸ� -- 
				m_xPos--;
			} else if(m_yPos>0){//�� �����̸� y�ø��� x������  
				m_yPos--;
				m_xPos=m_ScreenWidth-1;
			}
			
			*(m_pVideoMemory+t)=' ' | ((unsigned char)m_Color<<8);//���� ��ġ�� ������ ����(�ؽ�Ʈ ���������� ����) 
			break;
		
		default:
			if(c<' ')//����� ���ڰ� �ƽ�Ű ���ڰ� �ƴ϶�� ���� 
				break; 
				
			ushort* VideoMemory=m_pVideoMemory+m_ScreenWidth*m_yPos+m_xPos;//���ڰ� ��µ� ���� ��ġ�� ���
			uchar attribute=(uchar)((backColour<<4)||(textColour&0xF));//�������� ���(4)+����(4)
			
			*VideoMemory=(c | (ushort)(attribute<<8));//���� ������ġ���� ���ڿ� ������ �����Ͽ� ����.
			m_xPos++;//Ŀ��+1
			break; 
	}
	
	//Write���� ȭ���̵� �ʿ�� ����. 
	if(m_xPos>=m_ScreenWidth)
		m_yPos++;
	if(m_yPos==m_ScreenHeight){
		scrollup();
		m_yPos--;
	}
	//Ŀ���̵� 
	MoveCursor(m_xPos+1, m_yPos);
}
