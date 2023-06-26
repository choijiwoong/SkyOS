//PIC(Programmable Interrupt Controller): �̺�Ʈ ���ͷ�Ʈ �� PIC�� CPU���� ��ȣ����� INT�� ������, CPU�� EFLAG�� IE��Ʈ�� ���� �� PIC���� �����߻����޿Ϸ��ȣ�� ������. �׵� PIC�� INTA�� ���� ���ͷ�Ʈ ��ó�� 
//�����͹����� ���� CPU���� �����ϰ�, CPU�� ��ȣ����� IDT ��ũ���͸� ã�� ���ͷ�Ʈ �ڵ鷯�� �����Ѵ�. �����̺꿡�� ���ͷ�Ʈ �� ������ PIC IRQ 2���� ���ͷ�Ʈ ��ȣ�� ������.
 //�� ��ġ�� ���ͷ�Ʈ�� �ǹ��ϴ� IRQ���� ���� �ʱ�ȭ������Ѵ�. �̶� ICW�� ����Ѵ�. 
void PICInitialize(uint8_t base0, uint8_t base1){
	uint8_t icw=0;//ICW(Initialization Control Word)�� ������ PIC�� ��ɷ������ͷ� ����� �����ϴµ��� ����Ѵ�.
	
	icw=(icw & ~I86_PIC_ICW1_MASK_INIT) | I86_PIC_ICW1_INIT_YES;//PIC �ʱ�ȭ
	icw=(icw & ~I86_PIC_ICW1_MASK_IC4) | I86_ICW1_IC4_EXPECT;
	
	SendCommandToPIC(icw, 0);//ICW1��ɾ�(PIC�ʱ�ȭ)�� ������0�� �����̺�1�� ����.  
	SendCommandToPIC(icw, 1); 
	
	SendDataToPIC(base0, 0);//IRQ�� ���̽� �ּ��� base0�� base1�� ���� ICW2���(IRQ�������� ���� ���̽��ּ�)�� ������. 
	SendDataToPIC(base1, 1);//�����Ϳ� �����̺꿡�� ������ ���̽��ּ� ���� 
	
	SendDataToPIC(0x04, 0);//PIC�� ICW3���(�����Ϳ� �����̺� ������ ����)�� ������. �����Ϳ� �����̺� PIC���� ���� ���� 
	SendDataToPIC(0x02, 1);//�����Ϳ� �����̺꿡�� ������ ���� 
	
	icw=(icw & ~I86_PIC_ICW4_MASK_UPM) | I86_PIC_ICW4_UPM_86MODE;//ICW4���(�߰����)�� ������. i86��� Ȱ��ȭ
	
	SendDataToPIC(icw, 0);//PIC �ʱ�ȭ �Ϸ� 
	SendDataToPIC(icw, 1); //�߰���ɾ �����Ϳ� �����̺꿡�� ����. 
}// ReadDataFromPIC: PIC���� 1byte�б�, SendDataToPIC: PIC�� ������ ����, SendCommandToPIC: PIC�� ��ɾ� ����. ���������� OutPortByte, OutPortWord, OutPortDWord, InPortByte, InPortWord, InPortDWord�� ����Ͽ� 1, 2, 4����Ʈ������ �а���. 

//ICW����� 4������ �ʱ�ȭ ��ɾ�� �����Ǵµ� 1���� 4���� ���ʷ� PIC�ʱ�ȭ,  PIC�� ���ͷ�Ʈ�� �޾��� �� IRQ�� �󸶸� ���� CPU���� ������ ����,
//PIC�� �����Ϳ� �����̺� ������ ����, �߰���ɾ�� �̷������. 
