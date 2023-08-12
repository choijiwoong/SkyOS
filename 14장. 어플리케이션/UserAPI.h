//�ý��� API �߰�
void sleep(int millisecond){
	__asm{
		mov ebx, millisecond
		mov eax, 7
		int 0x80
	}
} 

//���� API 
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
		mov ebx, sz; //�Ű������� ebx��, 
		mov eax, 2; //���� API��ȣ 2�� eax�� 
		int 0x80; //Ŀ�θ��� ��ȯ�Ǿ� 0x80���ͷ�Ʈ�� �ش��ϴ� ���ͷ�Ʈ �ڵ鷯�� ����ȴ�_���ͷ�Ʈ ȣ���ϸ� Ŀ�ο� �޸� �Ҵ� ��û
		mov address, eax; //������� address�� ���� 
	}
	return address;
}

//������ ��� �߰�
#define SOUND_MAGIC 0xB6

void Sound(UINT16 Frequency){
	OutPortByte(0x43, SOUND_MAGIC);
	
	Frequency=120000L / Frequency;
	OutPortByte(0x42, Frequency>>8);
	OutPortByte(0x61, InPortByte(0x61) | 3);
} 

void NoSound(){
	OutPortByte(0x61, InPortByte(0x61)&~3);
}

void Beep(){
	Sound(1000);
	msleep(100);
	NoSound();
}
