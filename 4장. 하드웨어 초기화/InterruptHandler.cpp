////default �ڵ鷯�� (I86_IRQ_HANDLER)InterruptDefaultHandler�� �ƴ� ���� Ŀ���� 
void SetInterruptVector(){//�⺻������ ���ͷ�Ʈ �߻� �� �������������� Program Counter�� Ŀ�� ���ÿ� ������ �� ���ͷ�Ʈ ���� ȣ�� �� �����Ͽ� �簳�Ѵ�. 
	setvect(0, (void(__cdecl&)(void))kHandleDivideByZero);
	setvect(3, (void(__cdecl&)(void))kHandleBreakPointTrap);//�ߴ��� 
	setvect(4, (void(__cdecl&)(void))kHandleOverflowTrap);//������� �����÷ο� 
	setvect(6, (void(__cdecl&)(void))kHandleInvalidOpcodeFault);//�Ǽ��� �ڵ尡 �ƴ� �����ͺκ��� �������� �� 
	setvect(13, (void(__cdecl&)(void))kHandleGeneralProtectionFault);//�Ϲݺ�ȣ���� 
	setvect(14, (void(__cdecl&)(void))kHandlePageFault);//������ ��Ʈ 
	.....
} 

void HandleDivideByZero(registers_t regs){//�������͵��� �޾ƿͼ� 
	kExceptionMessageHeader();
	SkyConsole::Print("Divide by 0 at Address[0x%x:0x%x]\n", regs.cs, regs.eip);//cs�� eip�ּҸ� ���(0 division ���� �߻� ��ġ) 
	SkyConsole::Print("EFLAGS[0x%x]\n", regs.eflags);//�������͵��� error flag���� ��� 
	SkyConsole::Print("ss : 0x%x\n", regs.ss);//���׸�Ʈ���������� SS�� ���ÿ����� ����Ų��. Stack Segment 
	for(;;);
}
/*
Divide-by-zero Error: ������� �ʹ�Ŀ�� ���� �������Ϳ� ���� �� ���� ��
Bound Range Exceeded: BOUND��ɾ�� �迭�� ���Ѱ� ���ѻ����� �ε����z ����Ͽ� ���ϴµ�, �� ��踦 �ʰ����� ���
Invalid Opcode: ���μ����� ��ȿ���� �ʰų� ���ǵ��� �ʴ� OP�ڵ� �����. 15����Ʈ �ʰ� ��ɽÿ��� �߻� 
Device Not Avaliable: ��Ƽ�½�ŷ �� �������α׷��� FPU�� XMM���� CR0�� �̿��� FPU�� ���� ��ó�� �����Ͽ� �����ٸ� ����
Invalid TSS: ��ȿ���� ���� ���׸�Ʈ �����Ͱ� �½�ũ�� �κ����� �����ǰų�, TSS���� SS�����͸� ����� �� ��ȿ���� ���� ���� ���׸�Ʈ ����
Segment Not Present: Present��Ʈ�ʵ尡 0�� ���׸�Ʈ�� ����Ʈ �ε�õ��� �߻�.
Stack-Segment Fault: �������� �ʴ� ���׸�Ʈ ��ũ���͸� �����ϴ� ���� ���׸�Ʈ ������ ���� �߻�(���� ���ѹ��� üũ ����) 
General Protection Fault: ���׸�Ʈ ����, DPL����, ���෹������ ����, NULL��ũ���� ����, ����x �������� ���� ��
Alignment Check: ���ĵ��� �ʴ� �޸� ������ ������ ����� ��
SIMD Floating-Point Exception: ����ũ���� ���� 128��Ʈ �̵�� �ε��Ҽ��� ���� Ȥ�� CR4.OSXMMEXCPT��Ʈ������.
Debug: �Ϲݰ�������(����), �������б⾲���ߴ���, IO�б�Ǵ��ߴ���, �̱۽���(��ǻ�͸� �ѹ��� �ϳ��� ��ɾ�� ����), �½�ũ ����Ī 
BreakPoint: �ߴ��� ����� INT3��ɾ�� ��ü�ߴٰ� Ʈ���Ǹ� INT3�� �ٽ� ������ ��ɾ�� �ٲ۴�.(��ɾ� ī��� --�� �̿�) 
Double Fault: ���ܸ� �ٷ� �� ���ų� ���� �ڵ鷯�� ����Ǵ� ���� ���ܰ� �߻��� ��. ���� �Ұ���
Triple Fault: ������Ʈ ���� �߻��� ���μ����� �����ع��� 
Page Fault: �����޸𸮿� ������ ���丮or���̺��� ���� ��, DPLȤ�� ������ ���� ��, ������ ���丮or���̺��� ��Ʈ�� �����Ʈ�� 1�� �� 
*/
