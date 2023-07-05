//IDE->SATA->SSD로 발전중이지만, IDE를 다룬다.
class HardDiskHandler{
	private:
		Collection<HDDInfo*> HDDs;//인식한 하드 디스크 정보모음
		static BYTE DoSoftwareReset(UINT16 deviceController);//리셋 
		BYTE m_lastError;//에러 
	
	public:
		void Initialize();
		
		BYTE* GetTotalDevices();
		HDDInfo* GetHDDInfo(BYTE* DPF);//특정 하드웨어 정보 
		
		UINT32 CHSToLBA(BYTE *DPF, UINT32 Cylinder, UINT32 Head, UINT32 Sector);
		void LBAToCHS(BYTE *DPF, UINT32 LBA, UINT32*Cylinder, UINT32*HEAD, UINT32*Sector);
		
		BYTE ReadSectors(BYTE* DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE* buffer, BOOLEAN WithRetry=TRUE);//CHS
		BYTE ReadSectors(BYTE* DPF, UINT32 StartLBASector, BYTE NoOfSectors, BYTE *buffer, BOOLEAN WithRetry=TRUE);//LBA
		BYTE WriteSectors(BYTE* DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE* buffer, BOOLEAN WithRetry=TRUE);//CHS
		.....
}; 

__declspec(naked) void _HDDInterruptHandler(){
	//아무것도 안하지롱. 제어권을 CPU에 돌린다. 
} 

typedef struct tag_HDDInfo{
	BYTE IORegisterIdx;//IO리소스 
	BYTE IRQ;//IRQ인덱스 번호 
	
	BYTE DeviceNumber;//0마스터 1슬레이브 
	char SerialNumber[21];//시리얼 넘버 
	char FirmwareRevision[21];//펌웨어 버전 
	char ModelNumber[21];//모델번호 
	BYTE DMASupported;//지원여부 
	BYTE LBASupported;
	
	BYTE DeviceID[512];//디바이스 식별 커맨드에서 얻은 디바이스 ID 
	BYTE Mode;//0CHS, 1LBA
	
	UINT16 CHSHeadCount;//헤드 수 
	UINT16 CHSCylinderCount;//실린더 수 
	UINT16 CHSSectorCount;//섹터 수 
	UINT16 LBACount;//LBA모드일 경우에 사용 
	UINT16 BytesPerSector;//섹터당 바이트수 
	
	BYTE LastError;
} HDDInfo; 

#define IDE_CONTROLLER_NUM 1 //최대 4개까지 가능. 

void HardDiskcHandler::Initialize(){
	char strKey[3]="H0";//하드디스크 ID
	
	setvect(32+14, _HDD_InterruptHandler);//핸들러 등록 
	setvect(32+15, _HDD_InterruptHandler);
	
	HDDs.Initialize();//인식한 하드디스크 리스트 컬렉션 목록을 초기화
	
	for(int DeviceController=0; DeviceController<IDE_CONTROLLER_NUM; DeviceController++){//여기서 의미하는 디바이스 컨트롤러는 수용가능한 디바이스 개수만큼의 인덱스인듯. 
		DoSoftwareReset(DeviceController);//소프트웨어 리셋 
		if(IsDeviceControllerBusy(DeviceController, 1000))//사용불가시 패스 
			continue;
		
		OutPortByte(IDE_Con_IOBases[DeviceController][0]+IDE_CD_COMMAND, IDE_COM_EXECUTE_DEVICE_DIAGNOSTIC);//디바이스 진단코드 전송(하드 컨트롤러 기본입출력 0은 입력 1은 출력 ..아마 )
		BYTE result=InPortByte(IDE_Con_IOBases[DeviceController][0]+IDE_CB_ERROR);//에러 레지스터로부터 진단 결과를 얻어온다.
		
		for(BYTE device=0; device<2; device++){//디바이스의 마스터 슬레이브 각각에 대하여 작업 수행 
			UINT16 DeviceID_Data[512], j;//512바이트 씩 읽을 예정 
			if(device==1 && (result & 0x80))//디바이스 IO가 불가능하다면(위의 진단결과) 
				continue;
			
			if(device==1)//슬레이브라면
				OutPortByte(IDE_Con_IOBases[DeviceController][0]+IDE_CB_DEVICE_HEAD, 0x10);//무슨 역활일까..뭘 전달하는 걸깐 
			else
				OutPortByte(IDE_Con_IOBases[DeviceController][0]+IDE_CB_DEVICE_HEAD, 0x0); 
			msleep(50);
			
			//디바이스 정보요청
			OutPortByte(IDE_Con_IOBases[DeviceController][0]+IDE_CD_COMMAN, IDE_COM_IDENTIFY_DEVICE);
			if(!IsDevicieDataReady(DevicieController, 600, TRUE)){//대기 
				SkyConsole::Print("Data not ready %d\n", DeviceController);
				continue;
			}
			
			for(j=0; j<256; j++)//512바이트씩 읽기..인데 왜 256이지..? DeviceID_Data의 타입인 UINT16이 2바이트임. 
				DeviceID_Data[j]=InPortWord(IDE_Con_IOBases[DeviceController][0]+IDE_CB_DATA);
			
			HDDInfo* newHDD+(HDDInfo*)kmalloc(sizeof(HDDInfo));//HDD노드 생성 
			if(newHDD==NULL){
				SkyConsole::Print("HDD Initialize :: Allocation failed\n");
				return;
			}
			
			newHDD->IORegisterIdx=DeviceController;//노드에 디바이스 정보기록 
			memcpy(newHDD->DeviceID, DeviceID_Data, 512);
			newHDD->DeviceNumber=device;
			newHDD->LastError=0;
			newHDD->CHSSectorCount=DeviceID_Data[6];
			.....
			
			//구조체에 정보를 채운 뒤 하드디스크 목록에 추가
			HDDs.Add(newHDD, strKey);
			SkyConsole::Print("DeviceID: %x, %s\n", device, newHDD->ModuleNumber);
			skrKey[1]++;//새 하드 디스크 노드를 위해 하드디스크 ID를 변경 char[3]="H0"이기에 다음 0으로 넘어간다....으앙 
		}
	} 
	 
} 

BYTE HardDiskHandler::Read	ReadSectors(BYTE* DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE* buffer, BOOLEAN WithRetry){
	HDDInfo* pHDDInfo;
	BYTE DevHead, StartCylHigh=0, StartCylLow=0;
	
	pHDDInfo=HDDs.Item((char*)DPF);//하드웨어 정보 
	if(pHDDInfo==NULL){
		m_lastError=HDD_NOT_FOUND;
		return HDD_NOT_FOUNT;
	}
	
	if(pHDDInfo->DeviceNumber==0)//마스터 
		DevHead=StartHead|0xA0;
	else//슬레이브 
		DevHead=StartHead|0xB0;
		
	if(IsDeviceControllerBusy(pHDDInfo->IORegisterIdx, 1*60)){//디바이스가 준비될때까지 대기(정한 시간동안 내부적으로 기다리다가 답없으면 1리턴하는듯) 
		m_lastError=HDD_CONTROLLER_BUSY;
		return HDD_CONTROLLER_BUSY;
	}
	
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CD_DEVICE_HEAD, DevHead);//IORegisterIdx는 IDE_Con_IOBases즉 기본하드웨어입출력배열의 인덱스이다. 
	if(!IsDeviceDataReady(pHDDInfo->IORegisterIdx, 1*60, FALSE){//디바이스 헤드에 반응이 있는지 확인(데이터 커맨드를 받을 수 있는 상태인지)
		m_lastError=HDD_DATA_COMMAND_NOT_READY;
		return HDD_DATA_COMMAND_NOT_READY;
	}
	
	StartCylHigh=StartCylinder>>8;
	StartCylLow=(StartCylinder<<8)>>8;
	
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CB_CYLINDER_HIGH, StartCylHigh);//디바이스에 읽을 구역에 대한 정보 전송. (UINT16은 2바이트기에 분리해서 읽고 쓰고 함) 
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CB_CULINDER_LOW, StartCylLow);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CB_SECTOR, StartSector);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CB_SECTOR_COUTN, NoOfSectors);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CB_COMMAND, WithRetry ? IDE_COM_READ_SECTORS_W_RETRY : IDE_COM_READ_SECTORS);
	
	for(BYTE j=0; j<NoOfSectors; j++){//읽을 섹터별로 
		if(!IsDeviceDataReady(pHDDInfo->IORegisterIdx, 1*60, TRUE)){//읽을 준비가 안되었다면 
			m_lastError=HDD_DATA_NOT_READY;
			return HDD_DATA_NOT_READY;
		}
		
		for(UINT16 i=0; i<(pHDDInfo->BytesPerSector)/2; i++){//섹터별로 2바이트단위 정보읽기. 
			UINT16 w=0;
			BYTE l, h;
			
			w=InPortWord(IDE_Con_IOBases[pHDDInfo->IORegisterIdix][0] + IDE_CB_DATA);//데이터를 2바이트씩 읽는다. 
			l=(w<<8)>>8;//low부와 
			h=w>>8;//high부로 바이트씩으로 바꾸고 
			
			buffer[(j*(pHDDInfo->BytesPerSector))+(i*2)]=l;//char(1바이트)별로 딱딱 저장. 
			buffer[(j*(pHDDInfo->BytesPerSector))+(i*2)+1]=h;
		}
	}
	return HDD_NO_ERROR;
}
