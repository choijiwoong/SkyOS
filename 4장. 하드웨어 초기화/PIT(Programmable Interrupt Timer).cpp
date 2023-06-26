//PIT(Programmable Interrupt Timer): �Ϲ����� Ÿ�̸� ��� ���� ������� Ÿ�̸� ���ͷ�Ʈ ���·� CPU�� 32�� ���ͷ�Ʈ ��ȣ�� �����Ѵ�. 
void InitializePIT(){//PIT�ʱ�ȭ(���) 
	setvect(32, InterrputPITHandler);//PIT�ڵ鷯�� CPU�� 32�� ���ͷ�Ʈ ���ͷ� �����Ѵ�. 
}

void StartPITCounter(uint32_t freq, uint8_t counter, uint8_t mode){//������, ����� ī���� ��������(0,1,2,3�� ����), ī���� ��� 
	if(freq==0)//�������� 0�̸� ���� 1s/freq���� ����Ǵ�.. 0division 
		return;
	
	uint16_t divisor=uint16_t(1193181 / (uint16_t)freq);//PIT�� �ʴ�ƽ 1193181�̱⿡ �ð�����(ƽ) ��ꤤ 
	
	uint8_t ocw=0;//����� ��ɾ� �ʱ�ȭ 
	ocw=(ocw & ~I86_PIT_OCW_MASK_MODE) | mode;//ī���� ������� PIT �ʱ�ȭ 
	ocw=(ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;//�𸣰ڴ�.. 
	ocw=(ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;//����� ī���� �������� ���� 
	SendPICCommand(ocw);//PIC���� PIT Ŀ�ǵ� ����
	
	SendPITData(divisor & 0xff, 0);//�������� 32�� ���ͷ�Ʈ �ڵ鷯�� ȣ��..?
	SendPITData((divisor >> 8) & 0xff, 0);//�������� ���� ���� 
	
	_pitTicks=0; //Ÿ�̸�(ƽ ī��Ʈ) ���� 
}

//�Ʒ��� ���� ���
StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);//ī���� 0~2�� �а��Ⱑ �����ϰ�, 3�� ���⸸ �����ϴ�. 
