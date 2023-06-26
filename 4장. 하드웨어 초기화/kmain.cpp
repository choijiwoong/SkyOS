//실제 타이머 실습 코드
void kmain(unsigned long magic, unsigned long addr){
	SkyConsole::Initialize();
	
	SkyConsole::Print("*** Sky OS Console System Init ***\n");
	
	kEnterCriticalSection();//lock 
	
	HardwareInitiize();
	SkyConsole::Print("Hardware Init Complete\n");
	.....
	
	kLeaveCriticalSection();
	
	//타이머 시작
	StartPICCounter(100, I86_PIT_OCW_COUTNER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN); 
}

void HardwareInitiize(){
	GDTInitialize();//Global Descriptor Table초기화 
	IDTInitialize(0x8);//Interrupt Descriptor Table초기화 
	PICInitialize(0x20, 0x28);//Programmable Interrupt Countroller초기화(예외 처리 컨트롤러 IRQ)
	InitializePIT();//Programmable Interrupt Timer초기화
}

//InterruptPITHandler 프로시저
__declspec(naked) void InterruptPITHandler(){//PIT초기화 시 넣은 인터럽트핸들러(타이머 될때마다 실행시킬 함수)이다. 진동수가 100마다 즉 1초마다 수행된다. 
	.....
	if(_pitTicks - _lastTickCount >= 100){
		_lastTickCount=_pitTicks;
		SkyConsole::Print("Timer Count : %d\n", _pitTicks);
	}
	_pitTIcks++;
	.....
} 
