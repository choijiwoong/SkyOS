#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
//GRUB가 요구하는 시그니처(멀팁트 헤더 구조체의 명세) 
_declspec(naked) void multiboot_entry(void){
	__asm{//dd는 define double word의 매그로로 4바이트의 공간 확보 
		align 4//4바이트 정렬 
		multiboot_header:
			dd(MULTIBOOT_HEADER_MAGIC); magic number
			dd(MULTIBOOT_HEADER_FLAGS); flags
			dd(CHECKSUM); checksum
			dd(HEADER_ADRESS); //헤더주소 KERNEL_LOAD_ADDRESS+ALIGN (0x100400).
			//즉, KERNEL_LOAD_ADDRESS는 0x100000영역인데 AliGN이 4바이트를 따르니 로드된 주소 다음이 KERNEL_HEADER_ADDRESS가 된다. 0x100400고정. 
			dd(KERNEL_LOAD_ADDRESS);//커널이 로르된 주소
			dd(00);
			dd(00);
			dd(HEADER_ADRESS+0x20);//커널 시작 주소. kernel_entry. 
		
		kernel_entry:
			mov esp, KERNEL_STACK;//스택 설정 
			
			push 0;//플레그 레지스터의 초기화 
			popf
			
			//GRUB의 정보값을 푸쉬 
			push ebx;//Multiboot_Info구조체의 포인터 
			push eax;//매직넘버 
			
			call kmain;//위의 두 파라미터를 기반으로 kmain함수를 호출한다. 
			
			halt://kmain이 리턴될 때까지 수행x 
				jmp halt;
	}
} 

//MULTIBOOT_HEADER구조체(위의 명세를 따른)
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

void kmain(unsigned long magic, unsigned long addr){//매직넘버, Multiboot_info구조체의 포인터
	InitializeConstructor();//글로벌 객체 초기화
	SkyConsole::Initialize();//화면에 문자를 찍기위한 초기화
	SkyConsole::Print("Hello world!\n") ;
	for(;;);//메인함수의 진행을 막는용도.. 
}

//***************kmain내부*******************
namespace SkyConcole{//콘솔 클래스 인터페이스(80*25 콘솔로거. 싱글턴 객체 느낌으로 사용하기 위해 네임스페이스 사용) 
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
	if(c==0x30){//비디오 카드가 흑백이라면 
		m_pVideoMemory=(unsigned short*)0xb0000;
		m_VideoCardType=VGA_MONO_CRT_ADDRESS;//비디오카드타입을 흑백으로 설정 
	} else{//비디오 카드가 컬러라면 
		m_pVideoMemory=(unsigned short*)0xb8000;
		m_VideoCardType=VGA_COLOR_CRT_ADDRESS;//비디오카드타입을 컬러로 설정 
	}
	
	//화면사이즈 80x25 
	m_ScreenHeight=25;
	m_ScreenWidth=80;
	
	//커서 위치 초기화 
	m_xPos=0;
	m_yPos=0;
	
	//색상설정 
	m_Text=White;
	m_backGroundColor=Black;
	m_Color=(ConsoleColor)((m_backGroundBolor<<4)|m_text);//앞의 4비트는 배경색, 뒤에 4비트는 텍스트 색상
	
	Clear(); 
}

void SkyConsole::Print(const char* str, ...){
	va_list args;
	va_start(args, str);//args가 첫번째 인수를 가리키도록 초기화 
	for(size_t i=0; i<strlen(str); i++){
		switch(str[i]){
			case '%':
				switch(str[i+1]){
					case 'c': {//문자 
						char c=va_arg(args, char);//인자를 가져옴 
						WriteChar(c, m_Text, m_backGroundColor);
						i++;
						break;
					}
						
					case 's': {//문자열
						int c=(int&)va_arg(args, char);//아스키코드 
						char str[256];
						strcpy(str, (const char*)c);
						Write(str);
						i++;
						break;
					}
					
					case 'd':
					case 'i': {//정수 
						int c=va_arg(args, int);
						char str[32]={0};
						itoa_s(c, 10, str);//char* "1000"을 int 1000으로 
						Write(str);
						i++;
						break;
					}
					
					case 'x': {//16진수 
						unsigned int c=va_arg(args, unsigned int);
						char str[32]={0};
						itoa_s(c, 16, str);
						Write(str);
						i++;
						break;
					}
					
					case 'f': {//4장의 FPU init 이후 추가
						double double_temp;
						double_temp=va_arg(args, double);
						char buffer[512];
						ftoa_fixed(buffer, double_temp);//double을 char*로 바꿈 
						Write(buffer);
						i++;
						break;
					}
						
					
					default://%t처럼 이상한거 들어오면 강제종료 
						va_end(args);
						return;
					}
				break;
			
			default://%d같은게 아닌 일반문자는 그냥 출력 
				WriteChar(str[i], m_Text, m_backGroundColor);
				break;
		}
	} 
	va_end(args);
}

void SkyConsole::WriteChar(char c, ConsoleColor textColor, ConsoleColor backColor){
	int t;
	switch(c){
		case '\n'://줄바꿈이면 
			m_xPos=0;
			m_yPos++;//마우스 새라인이동 
			break;
			
		case 8://백스페이스면
			t=m_xPos+m_yPos*m_ScreenWidth;//지울 현재 커서의 위치를 저장 
			if(t>0)//일반적인 경우라면 
				t--;//앞으로 이동(덮어씌울 시작점) 
			
			if(m_xPos>0){//맨 왼쪽이 아니면 -- 
				m_xPos--;
			} else if(m_yPos>0){//맨 왼쪽이면 y올리고 x끝으로  
				m_yPos--;
				m_xPos=m_ScreenWidth-1;
			}
			
			*(m_pVideoMemory+t)=' ' | ((unsigned char)m_Color<<8);//지울 위치에 공백을 대입(텍스트 색상정보도 삽입) 
			break;
		
		default:
			if(c<' ')//출력할 문자가 아스키 문자가 아니라면 무시 
				break; 
				
			ushort* VideoMemory=m_pVideoMemory+m_ScreenWidth*m_yPos+m_xPos;//문자가 출력될 버퍼 위치를 계산
			uchar attribute=(uchar)((backColour<<4)||(textColour&0xF));//색상정보 배경(4)+문자(4)
			
			*VideoMemory=(c | (ushort)(attribute<<8));//현재 버퍼위치값에 문자에 색상을 세팅하여 쓴다.
			m_xPos++;//커서+1
			break; 
	}
	
	//Write이후 화면이동 필요시 수행. 
	if(m_xPos>=m_ScreenWidth)
		m_yPos++;
	if(m_yPos==m_ScreenHeight){
		scrollup();
		m_yPos--;
	}
	//커서이동 
	MoveCursor(m_xPos+1, m_yPos);
}
