//유저 API 
uint32_t GetTickCount();
void free(void *p);
u32int malloc(u32int sz);
void CreateHeap();
void TerminateProcess();
extern "C" void printf(const char* szMsg, ...);
char GetKeyboardInput();

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
int __cdecl _purecall();
void operator delete[](void* p);
void operator delete(void *p, size_t size);

u32int malloc(u32int sz){
	int address=0;
	__asm{
		mov ebx, sz; //매개변수를 ebx에, 
		mov eax, 2; //서비스 API번호 2를 eax에 
		int 0x80; //커널모드로 전환되어 0x80인터럽트에 해당하는 인터럽트 핸들러가 실행된다_인터럽트 호출하며 커널에 메모리 할당 요청
		mov address, eax; //결과값을 address에 저장 
	}
	return address;
}
