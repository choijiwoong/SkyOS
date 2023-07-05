void FloppyDisk::Install(int irq){//Interrupt ReQuest
	setvect(irq, FloppyDiskHandler);
	Reset();
	ConfigureDriveData(13, 1, 0xf, true);//드라이브 정보 설정 
}

//등록된 대충 핸들러 
__declspec(naked) void FloppyDiskHandler(){
	_asm{
		PUSHAD
		PUSHFD
		CLI
	}
	
	_floppyDiskIRQ=true;//대충 예외가 발생했다~는 대충핸들러 
	SendEOI();
	
	_asm{
		POPFD
		POPAD
		IRETD
	}
}

//읽기함수
uint8_t ReadSector(int sectorLBA){//디스크 구조: 엑세스 암에 판독/기록 헤드가 달려있고, 전체 실린더의 층을 의미하는 것은 트랙, 층에서 읽는 최소 단위 크기를 섹터라고 한다. 
	if(_CurrentDrive>=4)
		return 0;
	
	int head=0, track=0, sector=1;//섹터는 512바이트 단위이며, 읽은 데이터는 DMA_BUFFER주소에 저장된다. 
	ConvertLBAToCHS(sectorLBA, head, track, sector);//LBA(Logical Block Addressing) 섹터가 일렬로 연결돼있다고 가정, CHS(Cylinder, Header, Sector) 물리적 데이터 블록에 주소를 제공 
	
	ControlMotor(true);
	if(Seek((uint8_t)track, (uin8_t)head) != 0)
		return 0;
	
	ReadSectorImpl((uint8_t)head, (uint8_t)track, (uint8_t)sector);
	ControlMotor(false);
	
	return (uint8_t*)DMA_BUFFER;
} 
