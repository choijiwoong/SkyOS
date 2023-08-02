/*
MINT64OS는 오픈소스 운영체제이다. MINT64(64비트 멀티코어 OS 원리와 구조_2011_한빛미디어)의 램디스크를 SkyOS로 가져온다. 
*/
bool RamDiskAdaptor::Initialize(){
	bool result=kInitializeRDDFileSystem();
	if(result==true){
		kGetHDDInformation(m_pInformation);//기본정보 설정 
		PrintRamDiskInfo();//기본정보 출력 
		return true;
	}
	return false;
}

int RamDiskAdaptor::GetCount(){ return 1; }//램디스크는 하나이다. 

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
	return (-1!=kCloseFile((MFILE*)file->_id));//아마 MINT64코드 호환성 맞춘듯 
}

PFILE RamDiskAdaptor::Open(const char* fileName, const char* mode){
	MFILE* pMintFile=kOpenFile(fileName, mode);//MINT64의 파일구조체 타입을 SkyOS의 파일구조체 타입으로 복사하여 공통된 인터페이스 제공 
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
램디스트는 패키지 매니저(패키지 메이커 툴)를 이용하여 커널뒤에 붙여진 패키지 데이터가 존재하는지 확인 후, 존재하면 램디스크에 해당 내용을 쓴다.
패키지 데이터의 구조는 패키지 헤더, 패키지 아이템..., 데이터 로 구성된다. 
*/ 
#define MAXFILENAMELENGTH 10
typedef struct tag_PACKAGEHEADER{
	char vcSignature[16];
	DWORD dwHeaderSize;//패키지헤더+패키지아이템...사이즈(실질적 헤더 사이즈) 
}PACKAGEHEADER;

typedef struct tag_PACKAGEITEM{
	char vcFileName[MAXFILENAMELENGTH];
	DWORD dwFileLength;
}PACKAGEITEM;

bool RamDiskAdaptor::InstallPackage(){//패키지 데이터를 파싱하여 모든 파일 데이터를 램디스크로 복사 
	FILE* fp;
	PACKAGEITEM* pstItem=nullptr;
	UINT32 dwDataAddress=0;
	
	//패키지 시그니처를 찾는다"SKYOS32PACKAGE"
	PACKAGEHEADER* pstHeader=FindPackageSignature(KERNEL_LOAD_ADDRESS, PhysicalMemoryManager::GetKernelEnd());//패키지 헤더 
	//패키지 데이터 포인터
	dwDataAddress=(UINT32)(((char*)pstHeader) + pstHeader->dwHeaderSize);//패키지 데이터 
	//패키지 아이템 구조체 포인터
	pstItem=(PACKAGEITEM*)(((char*)pstHeader) + sizeof(PACKAGEHEADER));//패키지 아이템 
	//패키지 아이템 개수
	DWORD dwItemCount=(pstHeader->dwHeaderSize - sizeof(PACKAGEHEADER)) / sizeof(PACKAGEITEM); 
	
	for(DWORD i=0; i<dwItemCount; i++){//패키지 아이템의 정보들을 읽어 패키지데이터에 복사 
		fp=fopen(pstItem[i].vcFileName, "w");
		fwrite((BYTE*)dwDataAddress, 1, pstItem[i].dwFileLength, fp);
		fclose(fp);
		dwDataAddress+=pstItem[i].dwFileLength;
	} 
	return true;
}
