//���� Ÿ�̸� �ǽ� �ڵ�
void kmain(unsigned long magic, unsigned long addr){
	SkyConsole::Initialize();
	
	SkyConsole::Print("*** Sky OS Console System Init ***\n");
	
	kEnterCriticalSection();//lock 
	
	HardwareInitiize();
	SkyConsole::Print("Hardware Init Complete\n");
	.....
	
	kLeaveCriticalSection();
	
	//Ÿ�̸� ����
	StartPICCounter(100, I86_PIT_OCW_COUTNER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN); 
}

void HardwareInitiize(){
	GDTInitialize();//Global Descriptor Table�ʱ�ȭ 
	IDTInitialize(0x8);//Interrupt Descriptor Table�ʱ�ȭ 
	PICInitialize(0x20, 0x28);//Programmable Interrupt Countroller�ʱ�ȭ(���� ó�� ��Ʈ�ѷ� IRQ)
	InitializePIT();//Programmable Interrupt Timer�ʱ�ȭ
}

//InterruptPITHandler ���ν���
__declspec(naked) void InterruptPITHandler(){//PIT�ʱ�ȭ �� ���� ���ͷ�Ʈ�ڵ鷯(Ÿ�̸� �ɶ����� �����ų �Լ�)�̴�. �������� 100���� �� 1�ʸ��� ����ȴ�. 
	.....
	if(_pitTicks - _lastTickCount >= 100){
		_lastTickCount=_pitTicks;
		SkyConsole::Print("Timer Count : %d\n", _pitTicks);
	}
	_pitTIcks++;
	.....
} 
