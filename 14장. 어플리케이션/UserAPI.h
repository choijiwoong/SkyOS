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
