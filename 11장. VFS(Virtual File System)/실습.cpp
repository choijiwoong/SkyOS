#include "Storage Manager.cpp"
#include "FileSysAdaptor.cpp"
#include "HDDAdaptor.cpp"
#include "RamDiskAdaptor.cpp"
#include "MemoryResourceAdaptor.cpp"

//저장장치 테스트
void TestStorage(const char* filename, char driveLetter); 

//VFS구성
bool StorageManager::ConstructFileSystem(multiboot_info* info){
	//HDD하드디스크 
	FileSysAdaptor* pHDDAdaptor=new HDDAdaptor("HardDisk", 'C');//플로피A 하드C 램K 메모리리소스L 
	pHDDAdaptor->Initialize();//초기화
	 
	if(pHDDAdaptor->GetCount()>0)
		StorageManager::GetInstance()->RegisterFileSystem(pHDDAdaptor, 'C');//저장장치매니저에 저장장치 등록 
	else
		delete pHDDAdaptor;
	
	//RAM 디스크
	FileSysAdaptor* pRamDiskAdaptor=new RamDiskAdaptor("RamDisk", 'K') ;
	if(pRamDiskAdaptor->Initialize()==true){
		StorageManager::GetInstance()->RegisterFileSystem(pRamDiskAdaptor, 'K');
		StorageManager::GetInstance()->SetCurrentFileSystemById('K');
		((RamDiskAdaptor*)pRamDiskAdaptor)->InstallPackage();
	} else{
		delete pRamDiskAdaptor;
	}
	
	//MemoeyResource 디스크
	FileSysAdaptor* pFloppyDiskAdaptor=new FloppyDiskAdaptor("FloppyDisk", 'A');
	if(pFloppyDiskAdaptor->Initialize()==true)
		StorageManager::GetInstance()->RegisterFileSystem(pFLoppyDiskAdaptor, 'A');
	else
		delete pFloppyDiskAdaptor;
	
	TestStorage("sample.txt", 'C');//등록이 제대로 되었다면, 읽을 수 있는지 확인 
	StorageManager::GetInstance()->SetCurrentFileSystemByID('K');
	SkyConsole::Print("K drive Selected\n");
	...
} 

void TestStorage(const char* filename, char driveLetter){//테스트 코드 
	StorageManager::GetInstance()->SetCurrentFileSystemByID(driveLetter);//현재 파일시스템으로 해당드라이브를 저장장치매니저 설정 
	FILE* pFile=fopen(filename, "r");//매니저에서 파일을 읽는다. 
	
	if(pFile!=NULL){//존재할 경우 출력
		SkyConsole::Print("Handle ID: %d\n", pFile->_id);
		BYTE* buffer=new BYTE[512];
		memset(buffer, 0, 512);
		int ret=fread(buffer, 511, 1, pFile);
		if(ret>0)
			SkyConsole::Print("%s\n", buffer);
		fclose(pFile);
		delete buffer;
	}
}
