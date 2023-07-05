//IDE->SATA->SSD�� ������������, IDE�� �ٷ��.
class HardDiskHandler{
	private:
		Collection<HDDInfo*> HDDs;//�ν��� �ϵ� ��ũ ��������
		static BYTE DoSoftwareReset(UINT16 deviceController);//���� 
		BYTE m_lastError;//���� 
	
	public:
		void Initialize();
		
		BYTE* GetTotalDevices();
		HDDInfo* GetHDDInfo(BYTE* DPF);//Ư�� �ϵ���� ���� 
		
		UINT32 CHSToLBA(BYTE *DPF, UINT32 Cylinder, UINT32 Head, UINT32 Sector);
		void LBAToCHS(BYTE *DPF, UINT32 LBA, UINT32*Cylinder, UINT32*HEAD, UINT32*Sector);
		
		BYTE ReadSectors(BYTE* DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE* buffer, BOOLEAN WithRetry=TRUE);//CHS
		BYTE ReadSectors(BYTE* DPF, UINT32 StartLBASector, BYTE NoOfSectors, BYTE *buffer, BOOLEAN WithRetry=TRUE);//LBA
		BYTE WriteSectors(BYTE* DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE* buffer, BOOLEAN WithRetry=TRUE);//CHS
		.....
}; 

__declspec(naked) void _HDDInterruptHandler(){
	//�ƹ��͵� ��������. ������� CPU�� ������. 
} 

typedef struct tag_HDDInfo{
	BYTE IORegisterIdx;//IO���ҽ� 
	BYTE IRQ;//IRQ�ε��� ��ȣ 
	
	BYTE DeviceNumber;//0������ 1�����̺� 
	char SerialNumber[21];//�ø��� �ѹ� 
	char FirmwareRevision[21];//�߿��� ���� 
	char ModelNumber[21];//�𵨹�ȣ 
	BYTE DMASupported;//�������� 
	BYTE LBASupported;
	
	BYTE DeviceID[512];//����̽� �ĺ� Ŀ�ǵ忡�� ���� ����̽� ID 
	BYTE Mode;//0CHS, 1LBA
	
	UINT16 CHSHeadCount;//��� �� 
	UINT16 CHSCylinderCount;//�Ǹ��� �� 
	UINT16 CHSSectorCount;//���� �� 
	UINT16 LBACount;//LBA����� ��쿡 ��� 
	UINT16 BytesPerSector;//���ʹ� ����Ʈ�� 
	
	BYTE LastError;
} HDDInfo; 

#define IDE_CONTROLLER_NUM 1 //�ִ� 4������ ����. 

void HardDiskcHandler::Initialize(){
	char strKey[3]="H0";//�ϵ��ũ ID
	
	setvect(32+14, _HDD_InterruptHandler);//�ڵ鷯 ��� 
	setvect(32+15, _HDD_InterruptHandler);
	
	HDDs.Initialize();//�ν��� �ϵ��ũ ����Ʈ �÷��� ����� �ʱ�ȭ
	
	for(int DeviceController=0; DeviceController<IDE_CONTROLLER_NUM; DeviceController++){//���⼭ �ǹ��ϴ� ����̽� ��Ʈ�ѷ��� ���밡���� ����̽� ������ŭ�� �ε����ε�. 
		DoSoftwareReset(DeviceController);//����Ʈ���� ���� 
		if(IsDeviceControllerBusy(DeviceController, 1000))//���Ұ��� �н� 
			continue;
		
		OutPortByte(IDE_Con_IOBases[DeviceController][0]+IDE_CD_COMMAND, IDE_COM_EXECUTE_DEVICE_DIAGNOSTIC);//����̽� �����ڵ� ����(�ϵ� ��Ʈ�ѷ� �⺻����� 0�� �Է� 1�� ��� ..�Ƹ� )
		BYTE result=InPortByte(IDE_Con_IOBases[DeviceController][0]+IDE_CB_ERROR);//���� �������ͷκ��� ���� ����� ���´�.
		
		for(BYTE device=0; device<2; device++){//����̽��� ������ �����̺� ������ ���Ͽ� �۾� ���� 
			UINT16 DeviceID_Data[512], j;//512����Ʈ �� ���� ���� 
			if(device==1 && (result & 0x80))//����̽� IO�� �Ұ����ϴٸ�(���� ���ܰ��) 
				continue;
			
			if(device==1)//�����̺���
				OutPortByte(IDE_Con_IOBases[DeviceController][0]+IDE_CB_DEVICE_HEAD, 0x10);//���� ��Ȱ�ϱ�..�� �����ϴ� �ɱ� 
			else
				OutPortByte(IDE_Con_IOBases[DeviceController][0]+IDE_CB_DEVICE_HEAD, 0x0); 
			msleep(50);
			
			//����̽� ������û
			OutPortByte(IDE_Con_IOBases[DeviceController][0]+IDE_CD_COMMAN, IDE_COM_IDENTIFY_DEVICE);
			if(!IsDevicieDataReady(DevicieController, 600, TRUE)){//��� 
				SkyConsole::Print("Data not ready %d\n", DeviceController);
				continue;
			}
			
			for(j=0; j<256; j++)//512����Ʈ�� �б�..�ε� �� 256����..? DeviceID_Data�� Ÿ���� UINT16�� 2����Ʈ��. 
				DeviceID_Data[j]=InPortWord(IDE_Con_IOBases[DeviceController][0]+IDE_CB_DATA);
			
			HDDInfo* newHDD+(HDDInfo*)kmalloc(sizeof(HDDInfo));//HDD��� ���� 
			if(newHDD==NULL){
				SkyConsole::Print("HDD Initialize :: Allocation failed\n");
				return;
			}
			
			newHDD->IORegisterIdx=DeviceController;//��忡 ����̽� ������� 
			memcpy(newHDD->DeviceID, DeviceID_Data, 512);
			newHDD->DeviceNumber=device;
			newHDD->LastError=0;
			newHDD->CHSSectorCount=DeviceID_Data[6];
			.....
			
			//����ü�� ������ ä�� �� �ϵ��ũ ��Ͽ� �߰�
			HDDs.Add(newHDD, strKey);
			SkyConsole::Print("DeviceID: %x, %s\n", device, newHDD->ModuleNumber);
			skrKey[1]++;//�� �ϵ� ��ũ ��带 ���� �ϵ��ũ ID�� ���� char[3]="H0"�̱⿡ ���� 0���� �Ѿ��....���� 
		}
	} 
	 
} 

BYTE HardDiskHandler::Read	ReadSectors(BYTE* DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE* buffer, BOOLEAN WithRetry){
	HDDInfo* pHDDInfo;
	BYTE DevHead, StartCylHigh=0, StartCylLow=0;
	
	pHDDInfo=HDDs.Item((char*)DPF);//�ϵ���� ���� 
	if(pHDDInfo==NULL){
		m_lastError=HDD_NOT_FOUND;
		return HDD_NOT_FOUNT;
	}
	
	if(pHDDInfo->DeviceNumber==0)//������ 
		DevHead=StartHead|0xA0;
	else//�����̺� 
		DevHead=StartHead|0xB0;
		
	if(IsDeviceControllerBusy(pHDDInfo->IORegisterIdx, 1*60)){//����̽��� �غ�ɶ����� ���(���� �ð����� ���������� ��ٸ��ٰ� ������� 1�����ϴµ�) 
		m_lastError=HDD_CONTROLLER_BUSY;
		return HDD_CONTROLLER_BUSY;
	}
	
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CD_DEVICE_HEAD, DevHead);//IORegisterIdx�� IDE_Con_IOBases�� �⺻�ϵ��������¹迭�� �ε����̴�. 
	if(!IsDeviceDataReady(pHDDInfo->IORegisterIdx, 1*60, FALSE){//����̽� ��忡 ������ �ִ��� Ȯ��(������ Ŀ�ǵ带 ���� �� �ִ� ��������)
		m_lastError=HDD_DATA_COMMAND_NOT_READY;
		return HDD_DATA_COMMAND_NOT_READY;
	}
	
	StartCylHigh=StartCylinder>>8;
	StartCylLow=(StartCylinder<<8)>>8;
	
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CB_CYLINDER_HIGH, StartCylHigh);//����̽��� ���� ������ ���� ���� ����. (UINT16�� 2����Ʈ�⿡ �и��ؼ� �а� ���� ��) 
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CB_CULINDER_LOW, StartCylLow);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CB_SECTOR, StartSector);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CB_SECTOR_COUTN, NoOfSectors);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0]+IDE_CB_COMMAND, WithRetry ? IDE_COM_READ_SECTORS_W_RETRY : IDE_COM_READ_SECTORS);
	
	for(BYTE j=0; j<NoOfSectors; j++){//���� ���ͺ��� 
		if(!IsDeviceDataReady(pHDDInfo->IORegisterIdx, 1*60, TRUE)){//���� �غ� �ȵǾ��ٸ� 
			m_lastError=HDD_DATA_NOT_READY;
			return HDD_DATA_NOT_READY;
		}
		
		for(UINT16 i=0; i<(pHDDInfo->BytesPerSector)/2; i++){//���ͺ��� 2����Ʈ���� �����б�. 
			UINT16 w=0;
			BYTE l, h;
			
			w=InPortWord(IDE_Con_IOBases[pHDDInfo->IORegisterIdix][0] + IDE_CB_DATA);//�����͸� 2����Ʈ�� �д´�. 
			l=(w<<8)>>8;//low�ο� 
			h=w>>8;//high�η� ����Ʈ������ �ٲٰ� 
			
			buffer[(j*(pHDDInfo->BytesPerSector))+(i*2)]=l;//char(1����Ʈ)���� ���� ����. 
			buffer[(j*(pHDDInfo->BytesPerSector))+(i*2)+1]=h;
		}
	}
	return HDD_NO_ERROR;
}
