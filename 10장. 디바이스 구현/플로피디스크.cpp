void FloppyDisk::Install(int irq){//Interrupt ReQuest
	setvect(irq, FloppyDiskHandler);
	Reset();
	ConfigureDriveData(13, 1, 0xf, true);//����̺� ���� ���� 
}

//��ϵ� ���� �ڵ鷯 
__declspec(naked) void FloppyDiskHandler(){
	_asm{
		PUSHAD
		PUSHFD
		CLI
	}
	
	_floppyDiskIRQ=true;//���� ���ܰ� �߻��ߴ�~�� �����ڵ鷯 
	SendEOI();
	
	_asm{
		POPFD
		POPAD
		IRETD
	}
}

//�б��Լ�
uint8_t ReadSector(int sectorLBA){//��ũ ����: ������ �Ͽ� �ǵ�/��� ��尡 �޷��ְ�, ��ü �Ǹ����� ���� �ǹ��ϴ� ���� Ʈ��, ������ �д� �ּ� ���� ũ�⸦ ���Ͷ�� �Ѵ�. 
	if(_CurrentDrive>=4)
		return 0;
	
	int head=0, track=0, sector=1;//���ʹ� 512����Ʈ �����̸�, ���� �����ʹ� DMA_BUFFER�ּҿ� ����ȴ�. 
	ConvertLBAToCHS(sectorLBA, head, track, sector);//LBA(Logical Block Addressing) ���Ͱ� �Ϸķ� ������ִٰ� ����, CHS(Cylinder, Header, Sector) ������ ������ ��Ͽ� �ּҸ� ���� 
	
	ControlMotor(true);
	if(Seek((uint8_t)track, (uin8_t)head) != 0)
		return 0;
	
	ReadSectorImpl((uint8_t)head, (uint8_t)track, (uint8_t)sector);
	ControlMotor(false);
	
	return (uint8_t*)DMA_BUFFER;
} 
