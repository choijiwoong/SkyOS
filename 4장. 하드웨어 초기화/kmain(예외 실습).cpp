//break point�� �ڵ� ù ����Ʈ �ڵ尡 0xcc�� ����ȴ�. �̰��� ������ BreakPointInterrupt�߻� 
void TestInvalidOpcode(void){
	__asm{
		align 4
		__asm _emit 0xcc//������ �극��ũ����Ʈ�� �����. 
		__asm _emit 0x00
		__asm _emit 0x00
		__asm _emit 0x00
	}
}

//Invalud Opcode�� ��ȿ���� ���� OPCODE�� ����ϸ� �ȴ�.
void TestInvalidOpcode(){
	__asm{
		align 4
		__asm _emit 0x00; //ù��° ��ɾ 0x00000000�̸� �ڵ尡 �ƴ� �����ͷ� �ؼ��ϱ⿡ CPU�� ��ȿ���� ���� ��ɾ�� �Ǵ��Ѵ�. 
		__asm _emit 0x00
		__asm _emit 0x00
		__asm _emit 0x00
	}
} 
 

int _divider=0;
int _dividend=100;
void TestDivideByZero(){
	int result=_dividend/_divider;
	//�Ʒ��� ������ �κм����� ������ �̹� result����� �������� ���� �ƴϱ⿡ �������� ����� ������� �ʴ´�.  
	if(_divider!=0)
		result=dividend/_divider;
	SkyConsole::Print("Result is %d, divider: %d\n", result, _divider); 
	/*������ ��Ʈ�� �ش��ϱ⿡ ���������� �����ϸ� ���������ϴ�.(ps. ������Ʈ�� abort�� �ش�Ǿ� �ڵ�����Ұ�) 
	HandleDivideByZero�޼��带 �ּ�ó���� �� �Ʒ��� ���� �ӽù������� �ڵ鸵�غ���
	extern int _divider;
	void HandleDivideByZero(registers_t regs){
		_divider=10;//0�̾����� 10���� ���� 
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
