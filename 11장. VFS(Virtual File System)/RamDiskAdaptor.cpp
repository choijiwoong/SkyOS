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
