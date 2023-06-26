//IDT(Interrupt Descriptor Table):  �ϵ���� ���ͷ�Ʈ Ȥ�� ���� ���ͷ�Ʈ(��Ʈ, abort, Ʈ��)�߻� �� ISR(Interrupt Service Routin)�� �ּҸ� ��ũ���ͼ�����+���������� ������ ���� �������ּҸ� ����.
typedef struct tag_idt_descriptor{
	USHORT offsetLow;//������ ���� 
	USHORT selector;//GDT�� ��ũ���͸� ã�� ���̽��ּҸ� ��´�. 
	BYTE reserved;//0
	BYTE flags;
	USHORT offsetHigh;//������ ���� 
} idt_desciptor; 

typedef struct tag_idtr{//IDTR �������Ϳ� ������ IDT���� 
	USHORT limit;
	UINT base;
} idtr; 

bool IDTInitialize(uint16_t codeSel){
	_idtr.limit=sizeof(idt_descriptor)*I86_MAX_INTERRUPTS-1;//���ͷ�Ʈ ����*���ͷ�Ʈ��ũ����ũ��� IDTR�� ũ�� ����
	_idtr.base=(uint32_t)&_idt[0];//�����ּ� �ѱ�� 
	
	memset((void*)&_idt[0], 0, sizeof(idt_desctriptor)*I86_MAX_INTERRUPTS-1);//ó���� �׻� NULL��ũ���� 
	fot(int i=0; i<I86_MAX_INTERRUPTS; i++)
		InstallInterruptHandler(i, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, (I86_IRQ_HANDLER)InterruptDefaultHandler);//�⺻ �ڵ鷯�� ���ͷ�Ʈ���񽺷�ƾ���� ��ġ 
		
	IDTInstall();//CPU���� IDT�� ��ġ�� �˷��ش�. 
	
	return true;
}

__seclspec(naked) void InterruptDefaultHandler(){
	_asm{
		PUSHAD; //�������͸� �����ϰ� 
		PUSHFD
		CLI; //���ͷ�Ʈ �÷��� Ŭ���� 
	}
	SendEOI(); //���ͷ�Ʈ ���� 
	_asm{
		POPFD; //�������� ���� 
		POPAD
		IRETD; //���Ϸ�ó�� �� ���� �½�ũ���� ���� 
	}
} 
