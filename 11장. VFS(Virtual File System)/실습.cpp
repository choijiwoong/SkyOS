#include "Storage Manager.cpp"
#include "FileSysAdaptor.cpp"
#include "HDDAdaptor.cpp"
#include "RamDiskAdaptor.cpp"
#include "MemoryResourceAdaptor.cpp"

//������ġ �׽�Ʈ
void TestStorage(const char* filename, char driveLetter); 

//VFS����
bool StorageManager::ConstructFileSystem(multiboot_info* info){
	//HDD�ϵ��ũ 
	FileSysAdaptor* pHDDAdaptor=new HDDAdaptor("HardDisk", 'C');//�÷���A �ϵ�C ��K �޸𸮸��ҽ�L 
	pHDDAdaptor->Initialize();//�ʱ�ȭ
	 
	if(pHDDAdaptor->GetCount()>0)
		StorageManager::GetInstance()->RegisterFileSystem(pHDDAdaptor, 'C');//������ġ�Ŵ����� ������ġ ��� 
	else
		delete pHDDAdaptor;
	
	//RAM ��ũ
	FileSysAdaptor* pRamDiskAdaptor=new RamDiskAdaptor("RamDisk", 'K') ;
	if(pRamDiskAdaptor->Initialize()==true){
		StorageManager::GetInstance()->RegisterFileSystem(pRamDiskAdaptor, 'K');
		StorageManager::GetInstance()->SetCurrentFileSystemById('K');
		((RamDiskAdaptor*)pRamDiskAdaptor)->InstallPackage();
	} else{
		delete pRamDiskAdaptor;
	}
	
	//MemoeyResource ��ũ
	FileSysAdaptor* pFloppyDiskAdaptor=new FloppyDiskAdaptor("FloppyDisk", 'A');
	if(pFloppyDiskAdaptor->Initialize()==true)
		StorageManager::GetInstance()->RegisterFileSystem(pFLoppyDiskAdaptor, 'A');
	else
		delete pFloppyDiskAdaptor;
	
	TestStorage("sample.txt", 'C');//����� ����� �Ǿ��ٸ�, ���� �� �ִ��� Ȯ�� 
	StorageManager::GetInstance()->SetCurrentFileSystemByID('K');
	SkyConsole::Print("K drive Selected\n");
	...
} 

void TestStorage(const char* filename, char driveLetter){//�׽�Ʈ �ڵ� 
	StorageManager::GetInstance()->SetCurrentFileSystemByID(driveLetter);//���� ���Ͻý������� �ش����̺긦 ������ġ�Ŵ��� ���� 
	FILE* pFile=fopen(filename, "r");//�Ŵ������� ������ �д´�. 
	
	if(pFile!=NULL){//������ ��� ���
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
