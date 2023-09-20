/*
1. VirtualBox의 자체디버거로 특정 주소에 breakpoint를 걸어 디버깅
2. DLL을 활용한 디버깅: 각 프로세스의 콜스택(스택을 거스르며 이전함수 복귀주소를 출력)을 가독성있게 출력해야한다. 로그나 덤프형식으로 외부에 전달 
	ㄴ파라미터,이전함수복귀주소,이전함수EBP값,로컬변수 / 파라미터2...동일구조 반복 
3. 정확한 디버깅을 위해, stacktrace과정에서 해당 주소가 가리키는 메서드를 맹글링되지 않은 이름으로 바꿀 수 있다.  
	ㄴWIN32로 모듈을 개발(파일입출력IO, 메모리할당, 화면출력)하면서 SkyOS에서도 동작할 수 있게 공통인 부분은 공유하고 공유되지 않은 영역은 인터페이스를 제공해서 각 플랫폼의 기능을 사용할 수 있게 한다. 
*/

//2. OS별 공유불가능한 기능을 위한 인터페이스_입출력관련(언맹글링 위함) 
typedef struct SKY_FILE_Interface{
	size_t (*sky_fread)(void* ptr, size_t size, size_t nmemb, FILE* stream);
	FILE* (*sky_fopen)(const char* filename, const char* mode);
	size_t (*sky_fwrite)(const void* ptr, size_t size, size_t nmemb, FILE* stream);
	int (*sky_fclose)(FILE* stream);
	int (*sky_feof)(FILE* stream);
	int (*sky_ferror)(FILE* stream);
	int (*sky_fflush)(FILE* stream);
	FILE* (*sky_freopen)(const char* filename, const char* mode, FILE* stream);
	int (*sky_fseek)(FILE* stream, long int offset, int whence);
	long int (*sky_ftell)(FILE* stream);
	int (*sky_fgetc)(FILE* stream);
	char* (*sky_fgets)(char* dst, int max, FILE* fp);
}SKY_FILE_Interface;

typedef struct SKY_ALLOC_Interface{//_메모리 할당 관련 
	unsigned int (*sky_kmalloc)(unsigned int sz);
	void (*sky_kfree)(void* p);
}SKY_ALLOC_Interface;

typedef struct SKY_Print_Interface{//_출력관련 
	void (*sky_printf)(const char* str, ...);
}SKY_Print_Interface;

class SkyMockInterface{//DLL로 넘길 입출력+메모리할당+출력관련 인터페이스 클래스 
	public:
		SKY_ALLOC_Interface g_allocInterface;
		SKY_FILE_Interface g_fileInterface;
		SKY_Print_Interface g_printInterface;
};

//1. 스택되감으며 함수 복귀주소 출력 
void SkyDebugger::TraceStack(unsigned int maxFrames){
	unsigned int* ebp=&maxFrames-2;//TraceStack함수를 실행시킨 호출함수의 복귀주소. [0]에는 호출함수 EBP값, ebp[1]호출함수 복귀주소가 담김. (로컬변수만큼 뺀듯) 
	SkyConsole::Print("Stack trace:\n");
	
	Thread* pTask=ProcessManager::GetInstance()->GetCurrentTask();
	for(unsigned int frame=0; frame<maxFrames; ++frame){
		unsigned int eip=ebp[1];//복귀주소 
		if(eip==0)//더 복귀할게 없으면 종료 
			break;
		ebp=reinterpret_cast<unsigned int*>(ebp[0]);//ebp를 다음 ebp로 갱신(거슬러이동) 
		unsigned int* arguments=&ebp[2];//매개변수도 저장 
		SkyConsole::Print(" 0x{%x}	\n", eip);//복귀주소를 출력 
	}
}
