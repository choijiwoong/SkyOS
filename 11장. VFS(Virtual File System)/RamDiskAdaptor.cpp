/*
MINT64OS�� ���¼ҽ� �ü���̴�. MINT64(64��Ʈ ��Ƽ�ھ� OS ������ ����_2011_�Ѻ��̵��)�� ����ũ�� SkyOS�� �����´�. 
*/
bool RamDiskAdaptor::Initialize(){
	bool result=kInitializeRDDFileSystem();
	if(result==true){
		kGetHDDInformation(m_pInformation);//�⺻���� ���� 
		PrintRamDiskInfo();//�⺻���� ��� 
		return true;
	}
	return false;
}

int RamDiskAdaptor::GetCount(){ return 1; }//����ũ�� �ϳ��̴�. 

void RamDiskAdaptor::PrintRamDiskInfo(){
	SkyConsole::Print("RamDisk Info\n");
	SkyConsole::Print("Total Sectors: %d\n", m_pInformation->dwTotalSectors);
	SkyConsole::Print("Serial Number: %s\n", m_pInformation->vwSerialNumber);
	SkyConsole::Print("Model Number: %s\n", m_pInformation->vwModelNumber);
}

int RamDiskAdaptor::Read(PFILE file, unsigned char* buffer, unsigned int size, int count){
	return kReadFile(buffer, size, count, (MFILE*)file->_id);
}
bool RamDiskAdaptor::Close(PFILE file){
	if(file==nullptr)
		return false;
	return (-1!=kCloseFile((MFILE*)file->_id));//�Ƹ� MINT64�ڵ� ȣȯ�� ����� 
}

PFILE RamDiskAdaptor::Open(const char* fileName, const char* mode){
	MFILE* pMintFile=kOpenFile(fileName, mode);//MINT64�� ���ϱ���ü Ÿ���� SkyOS�� ���ϱ���ü Ÿ������ �����Ͽ� ����� �������̽� ���� 
	if(pMintFile){
		PFILE file=new File;
		file->_deviceID='K';
		strcpy(file->_name, fileName);
		file->_if(DWORD)pMintFile;
		return file;
	}
	return nullptr;
}

size_t RamDiskAdaptor::Write(PFILE file, unsigned char* buffer, unsigned int size, int count){
	if(file==nullptr)
		return 0;
	return kWriteFile(buffer, size, count, (MFILE*)file->_id);
}

/*
����Ʈ�� ��Ű�� �Ŵ���(��Ű�� ����Ŀ ��)�� �̿��Ͽ� Ŀ�εڿ� �ٿ��� ��Ű�� �����Ͱ� �����ϴ��� Ȯ�� ��, �����ϸ� ����ũ�� �ش� ������ ����.
��Ű�� �������� ������ ��Ű�� ���, ��Ű�� ������..., ������ �� �����ȴ�. 
*/ 
#define MAXFILENAMELENGTH 10
typedef struct tag_PACKAGEHEADER{
	char vcSignature[16];
	DWORD dwHeaderSize;//��Ű�����+��Ű��������...������(������ ��� ������) 
}PACKAGEHEADER;

typedef struct tag_PACKAGEITEM{
	char vcFileName[MAXFILENAMELENGTH];
	DWORD dwFileLength;
}PACKAGEITEM;

bool RamDiskAdaptor::InstallPackage(){//��Ű�� �����͸� �Ľ��Ͽ� ��� ���� �����͸� ����ũ�� ���� 
	FILE* fp;
	PACKAGEITEM* pstItem=nullptr;
	UINT32 dwDataAddress=0;
	
	//��Ű�� �ñ״�ó�� ã�´�"SKYOS32PACKAGE"
	PACKAGEHEADER* pstHeader=FindPackageSignature(KERNEL_LOAD_ADDRESS, PhysicalMemoryManager::GetKernelEnd());//��Ű�� ��� 
	//��Ű�� ������ ������
	dwDataAddress=(UINT32)(((char*)pstHeader) + pstHeader->dwHeaderSize);//��Ű�� ������ 
	//��Ű�� ������ ����ü ������
	pstItem=(PACKAGEITEM*)(((char*)pstHeader) + sizeof(PACKAGEHEADER));//��Ű�� ������ 
	//��Ű�� ������ ����
	DWORD dwItemCount=(pstHeader->dwHeaderSize - sizeof(PACKAGEHEADER)) / sizeof(PACKAGEITEM); 
	
	for(DWORD i=0; i<dwItemCount; i++){//��Ű�� �������� �������� �о� ��Ű�������Ϳ� ���� 
		fp=fopen(pstItem[i].vcFileName, "w");
		fwrite((BYTE*)dwDataAddress, 1, pstItem[i].dwFileLength, fp);
		fclose(fp);
		dwDataAddress+=pstItem[i].dwFileLength;
	} 
	return true;
}
