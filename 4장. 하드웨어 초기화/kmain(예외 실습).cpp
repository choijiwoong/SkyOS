//break point는 코드 첫 바이트 코드가 0xcc로 변경된다. 이것을 실행히 BreakPointInterrupt발생 
void TestInvalidOpcode(void){
	__asm{
		align 4
		__asm _emit 0xcc//강제로 브레이크포인트를 만든다. 
		__asm _emit 0x00
		__asm _emit 0x00
		__asm _emit 0x00
	}
}

//Invalud Opcode는 유효하지 않은 OPCODE를 사용하면 된다.
void TestInvalidOpcode(){
	__asm{
		align 4
		__asm _emit 0x00; //첫번째 명령어가 0x00000000이면 코드가 아닌 데이터로 해석하기에 CPU는 유효하지 않은 명령어로 판단한다. 
		__asm _emit 0x00
		__asm _emit 0x00
		__asm _emit 0x00
	}
} 
 

int _divider=0;
int _dividend=100;
void TestDivideByZero(){
	int result=_dividend/_divider;
	//아래에 오동작 부분수정이 있지만 이미 result결과는 정상적인 값이 아니기에 정상적인 결과를 기대하진 않는다.  
	if(_divider!=0)
		result=dividend/_divider;
	SkyConsole::Print("Result is %d, divider: %d\n", result, _divider); 
	/*예외중 폴트에 해당하기에 에러수정이 가능하면 복구가능하다.(ps. 더블폴트는 abort에 해당되어 코드진행불가) 
	HandleDivideByZero메서드를 주석처리한 후 아래와 같이 임시방편으로 핸들링해보자
	extern int _divider;
	void HandleDivideByZero(registers_t regs){
		_divider=10;//0이었던걸 10으로 변경 
	} 
	 */
}

void TestInterrupt(){
	TestDivideByZero();
	//TestBreakPoint();
	//TestInvalidOpcode();
}

void kmain(unsigned long mafix, unsigned long addr){
	.....
	TestInterrupt();
} 
