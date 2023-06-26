//GDT(Global Descriptor Table): ��ȣ���忡�� ���� �ּҿ� ������ �������� �ּҹ����� ��ȿ��, DPL������� ���ٰ��ɿ��θ� Ȯ�����ִ� ����ü
typedef struct tag_gdtDescriptor{
	USHORT segmentLimit;//16��Ʈ ũ�� 
	USHORT baseLow;//���̽� �ּ� 
	BYTE baseMiddle;//�ּ� 
	BYTE flags;//grand���� �ּҸ� ����Ű�°� 
	BYTE grand;//4��Ʈ �߰�ũ��(���� 20��Ʈ�ε�, �̸� <<12�Ͽ� 32��Ʈ ���׸�Ʈ ũ�� �� 4GB�� ǥ���Ѵ�) 
	BYTE baseHigh;//�ּ� 
}gdtDescriptor; 

//GDTR(Global Descriptor Table Register): GPU�� GDTR�� �����Ͽ� GDT�� �������̰� ����� ��� lgdt�� GDTR�������Ϳ� ���� �����Ѵ�.
typedef struct tag_gdtr{//6����Ʈ GDTR����ü. lgdt gdtr;�� ����Ѵ�. 
	USHORT m_limit;//GDT�� ũ�� 
	UINT m_base;//GDT�� �����ּ� 
};gdtr 

//GDT����
int GDTInitialize(){//��� ��ũ���� ���׸�Ʈ ���̽��ּҸ� 0, ���׸�Ʈ ũ�⸦ 4GB�� ���� 
	//GDTR�� �ε�� _gdtr�� �ʱ�ȭ�Ѵ�.
	//_gdtr�� �ּҴ� ���� �����ּ��̸�, MAX_DESCRIPTOR�� 5��(NULL ��ũ����, Ŀ���ڵ� ��ũ����, Ŀ�ε����� ��ũ����, �����ڵ� ��ũ����, ���������� ��ũ����)�̴�. ũ��� �� 8����Ʈ�� GDT�� 40����Ʈ�̴�.
	_gdtr.m_limit=(sizeof(struct gdt_descriptor)*MAX_DESCRIPTOR)-1;
	_gdtr.m_base=(uint32_t)&_gdt[0];//�����ּҸ� GDT�� �����ּҷ�.
	
	//1. NULL ��ũ���� ����: ��ũ���� ���̺� ���� ù��° ��ũ���ʹ� �׻� NULL�� ���� 
	get_set_descriptor(0,0,0,0,0);
	
	//2. Ŀ���ڵ� ��ũ���� ����: Ŀ���ڵ���� �� ���ٱ����� ��� 
	gdt_set_descriptor(1,0,0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY, //�а��Ⱑ��, �ڵ弼�׸�Ʈ(default�� ������),  �ý������Ǽ��׸�Ʈ��0 �ڵ峪�����ͼ��׸�Ʈ�� 1, �� ���׸�Ʈ�� �����޷θ��� �ö���־� ���ٰ����ϴ� 
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);//���׸�Ʈ���� 4K, 32��Ʈ���, grand��Ʈ������ �� 4����Ʈ�� �����Ͽ� 20��Ʈ�� 4GB������ ǥ��. 
	
	//3. Ŀ�ֵ����� ��ũ���� ����: Ŀ�ֵ����Ϳ����� WR�Ҷ� ���ٱ����� ��� 
	gdt_set_descriptor(2,0,0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY,//�а��Ⱑ��, �ý������ǽ� 0 �ڵ峪�����͸� 1, �����޸𸮿� �ö���ֱ⿡ ���ٰ��� 
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);//���׸�Ʈ���� 4K, 32��Ʈ���, �߰��ּҵ����� 20bit�� 4GB 
	
	//4. ������� �ڵ� ��ũ���� ����: �����ڵ���� �� ���ٱ����� ��� 
	gdt_set_descriptor(3, 0, 0xffffffff
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY | I86_GDT_DESC_DPL,//�а��Ⱑ��, �ڵ弼�׸�Ʈ, �ý������ǽ�0 �ڵ峪�����͸�1, �����޸��̱⿡ ���ٰ���, DPLƯ�� ���� 
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);//���׸�Ʈ���� 4K, 32��Ʈ���, �߰��ּ� 
	
	//5. ������� ������ ��ũ���� ����: ���������Ϳ����� ���� �� ���ٱ����� ��� 
	gdt_set_descriptor(4,0,0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_DPL, //�о�, �ý������ǽ� 0 �ڵ峪�����͸� 1, DPLƯ�Ƿ��� 
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);//���׸�Ʈ ���� 4K, 32��Ʈ���, �߰��ּ� 
	
	//GDTR�������Ϳ� GDT �ε�. �� �� �÷��׷� I86_GDT_DESC_ACCESS(� �����尡 ���ٽ� 1_��������Ʈ), I86_GDT_DESC_EXPANSION(Ȯ���Ʈ), I86_GDT_GRAND_OS(64��Ʈ ����Ƽ�� �ڵ带 �����ϰ� �ִ°�) 
	gdt_install();
	
	return 0; 
} 
