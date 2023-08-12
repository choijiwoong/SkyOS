//커널 API

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
	setvect(0x80, SysCallDispatcher, I86_IDT_DESC_RING3);//0x80인터럽트 시 SysCallDispatcher함수를 호출한다. 
}

_declspec(naked) void SysCallDispatcher(){//스택프레임x 
	static uint32_t idx=0;
	_asm{
		push eax
		mov eax, 0x10
		mov ds, ax; //데이터 셀렉터를 0x10으로 변경
		pop eax
		mov[idx], eax; //서비스 API번호를 idx에 저장 
		pusha
	}
	
	id(idx>=MAX_SYSCALL){//서비스 API인덱스가 최댓값보다 클 경우 
		_asm{
			popa
			irete
		}
	}
	
	static void* fnct=0;
	fnct=_syscalls[idx];//_syscalls(SSDT, System Service Descriptor Table)에서 커널API를 가져온다 
	
	_asm{
		popa
		push edi; //필요한 인자들을 push하고 
		push esi
		push edx
		push ecx
		push ebx
		
		call fnct; //함수 호출. 5개 인자를 다 쓰지 않더라도 WinOS에서 커널모드 전환 이전에 인자를 검증하기에 무결성이 보증된다. 
		add esp, 20; //스택프레임을 생성하지 않기에 직접 스택을 정리해야하며, 위에서 5파라미터를 넣었기에 ESP를 20바이트 증가시킨다
		.....
		iretd
	}
}
