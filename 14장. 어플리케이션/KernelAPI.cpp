//Ŀ�� API

static void* _syscalls[]={
	kprintf,
	TerminateProcess,
	MemoryAlloc,
	MemoryFree,
	CreateDefaultHeap,
	GetTickCount,
	CreateThread,
};

void InitializeSysCall(){
	setvect(0x80, SysCallDispatcher, I86_IDT_DESC_RING3);//0x80���ͷ�Ʈ �� SysCallDispatcher�Լ��� ȣ���Ѵ�. 
}

_declspec(naked) void SysCallDispatcher(){//����������x 
	static uint32_t idx=0;
	_asm{
		push eax
		mov eax, 0x10
		mov ds, ax; //������ �����͸� 0x10���� ����
		pop eax
		mov[idx], eax; //���� API��ȣ�� idx�� ���� 
		pusha
	}
	
	id(idx>=MAX_SYSCALL){//���� API�ε����� �ִ񰪺��� Ŭ ��� 
		_asm{
			popa
			irete
		}
	}
	
	static void* fnct=0;
	fnct=_syscalls[idx];//_syscalls(SSDT, System Service Descriptor Table)���� Ŀ��API�� �����´� 
	
	_asm{
		popa
		push edi; //�ʿ��� ���ڵ��� push�ϰ� 
		push esi
		push edx
		push ecx
		push ebx
		
		call fnct; //�Լ� ȣ��. 5�� ���ڸ� �� ���� �ʴ��� WinOS���� Ŀ�θ�� ��ȯ ������ ���ڸ� �����ϱ⿡ ���Ἲ�� �����ȴ�. 
		add esp, 20; //������������ �������� �ʱ⿡ ���� ������ �����ؾ��ϸ�, ������ 5�Ķ���͸� �־��⿡ ESP�� 20����Ʈ ������Ų��
		.....
		iretd
	}
}
