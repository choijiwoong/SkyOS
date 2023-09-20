/*
1. VirtualBox�� ��ü����ŷ� Ư�� �ּҿ� breakpoint�� �ɾ� �����
2. DLL�� Ȱ���� �����: �� ���μ����� �ݽ���(������ �Ž����� �����Լ� �����ּҸ� ���)�� �������ְ� ����ؾ��Ѵ�. �α׳� ������������ �ܺο� ���� 
	���Ķ����,�����Լ������ּ�,�����Լ�EBP��,���ú��� / �Ķ����2...���ϱ��� �ݺ� 
3. ��Ȯ�� ������� ����, stacktrace�������� �ش� �ּҰ� ����Ű�� �޼��带 �ͱ۸����� ���� �̸����� �ٲ� �� �ִ�.  
	��WIN32�� ����� ����(���������IO, �޸��Ҵ�, ȭ�����)�ϸ鼭 SkyOS������ ������ �� �ְ� ������ �κ��� �����ϰ� �������� ���� ������ �������̽��� �����ؼ� �� �÷����� ����� ����� �� �ְ� �Ѵ�. 
*/

//2. OS�� �����Ұ����� ����� ���� �������̽�_����°���(��ͱ۸� ����) 
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

typedef struct SKY_ALLOC_Interface{//_�޸� �Ҵ� ���� 
	unsigned int (*sky_kmalloc)(unsigned int sz);
	void (*sky_kfree)(void* p);
}SKY_ALLOC_Interface;

typedef struct SKY_Print_Interface{//_��°��� 
	void (*sky_printf)(const char* str, ...);
}SKY_Print_Interface;

class SkyMockInterface{//DLL�� �ѱ� �����+�޸��Ҵ�+��°��� �������̽� Ŭ���� 
	public:
		SKY_ALLOC_Interface g_allocInterface;
		SKY_FILE_Interface g_fileInterface;
		SKY_Print_Interface g_printInterface;
};

//1. ���õǰ����� �Լ� �����ּ� ��� 
void SkyDebugger::TraceStack(unsigned int maxFrames){
	unsigned int* ebp=&maxFrames-2;//TraceStack�Լ��� �����Ų ȣ���Լ��� �����ּ�. [0]���� ȣ���Լ� EBP��, ebp[1]ȣ���Լ� �����ּҰ� ���. (���ú�����ŭ ����) 
	SkyConsole::Print("Stack trace:\n");
	
	Thread* pTask=ProcessManager::GetInstance()->GetCurrentTask();
	for(unsigned int frame=0; frame<maxFrames; ++frame){
		unsigned int eip=ebp[1];//�����ּ� 
		if(eip==0)//�� �����Ұ� ������ ���� 
			break;
		ebp=reinterpret_cast<unsigned int*>(ebp[0]);//ebp�� ���� ebp�� ����(�Ž����̵�) 
		unsigned int* arguments=&ebp[2];//�Ű������� ���� 
		SkyConsole::Print(" 0x{%x}	\n", eip);//�����ּҸ� ��� 
	}
}
