////default 핸들러인 (I86_IRQ_HANDLER)InterruptDefaultHandler가 아닌 직접 커스텀 
void SetInterruptVector(){//기본적으로 인터럽트 발생 시 레지스터정보나 Program Counter를 커널 스택에 저장한 후 인터럽트 서비스 호출 후 복원하여 재개한다. 
	setvect(0, (void(__cdecl&)(void))kHandleDivideByZero);
	setvect(3, (void(__cdecl&)(void))kHandleBreakPointTrap);//중단점 
	setvect(4, (void(__cdecl&)(void))kHandleOverflowTrap);//산술연산 오버플로우 
	setvect(6, (void(__cdecl&)(void))kHandleInvalidOpcodeFault);//실수로 코드가 아닌 데이터부분을 실행했을 시 
	setvect(13, (void(__cdecl&)(void))kHandleGeneralProtectionFault);//일반보호오류 
	setvect(14, (void(__cdecl&)(void))kHandlePageFault);//페이지 폴트 
	.....
} 

void HandleDivideByZero(registers_t regs){//레지스터들을 받아와서 
	kExceptionMessageHeader();
	SkyConsole::Print("Divide by 0 at Address[0x%x:0x%x]\n", regs.cs, regs.eip);//cs와 eip주소를 출력(0 division 에러 발생 위치) 
	SkyConsole::Print("EFLAGS[0x%x]\n", regs.eflags);//레지스터들의 error flag들을 출력 
	SkyConsole::Print("ss : 0x%x\n", regs.ss);//세그먼트레지스터중 SS는 스택영역을 가리킨다. Stack Segment 
	for(;;);
}
/*
Divide-by-zero Error: 결과값이 너무커서 최종 레지스터에 담을 수 없을 떄
Bound Range Exceeded: BOUND명령어는 배열의 상한과 하한사이의 인덱스틑 사용하여 비교하는데, 그 경계를 초과했을 경우
Invalid Opcode: 프로세서가 유효하지 않거나 정의되지 않는 OP코드 실행시. 15바이트 초과 명령시에도 발생 
Device Not Avaliable: 멀티태스킹 시 유저프로그램이 FPU나 XMM사용시 CR0을 이용해 FPU가 없는 것처럼 설정하여 스케줄링 가능
Invalid TSS: 유효하지 않은 세그먼트 셀렉터가 태스크의 부분으로 참조되거나, TSS에서 SS셀렉터를 사용할 떄 유효하지 않은 스택 세그먼트 참조
Segment Not Present: Present비트필드가 0인 세그먼트나 게이트 로드시도시 발생.
Stack-Segment Fault: 존재하지 않는 세그먼트 디스크립터를 참조하는 스택 세그먼트 셀렉터 사용시 발생(스택 제한범위 체크 실패) 
General Protection Fault: 세그먼트 에러, DPL에러, 예약레지스터 에러, NULL디스크립터 에러, 구현x 레지스터 에러 등
Alignment Check: 정렬되지 않는 메모리 데이터 참조가 실행될 때
SIMD Floating-Point Exception: 마스크되지 않은 128비트 미디어 부동소수점 실행 혹은 CR4.OSXMMEXCPT비트설정시.
Debug: 일반감지조건(오류), 데이터읽기쓰기중단점, IO읽기또는중단점, 싱글스텝(컴퓨터를 한번에 하나의 명령어만을 실행), 태스크 스위칭 
BreakPoint: 중단점 실행시 INT3명령어로 대체했다가 트랩되면 INT3을 다시 원래의 명령어로 바꾼다.(명령어 카운더 --를 이용) 
Double Fault: 예외를 다룰 수 없거나 혜외 핸들러가 수행되는 동안 예외가 발생할 떄. 복구 불가능
Triple Fault: 더블폴트 예외 발생시 프로세서를 리셋해버림 
Page Fault: 물리메모리에 페이지 디렉토리or테이블이 없을 때, DPL혹은 권한이 없을 때, 페이지 디렉토리or테이블의 엔트리 예약비트가 1일 떄 
*/
