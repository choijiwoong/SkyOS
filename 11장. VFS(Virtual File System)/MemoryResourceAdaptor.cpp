//램디스크와 패키지메니저가 필요한 파일을 커널에 적재하는 역활을 GRUB단에서 자동로드할 수 있도록 매니저를 만들어서도 처리가 가능하다. 
class MemoryResourceAdaptor: public FileSysAdaptor{
	public:
		MemoryResourceAdaptor(char* deviceName, DWORD deviceID);
		~MemoryResourceAdaptor();
		
		virtual bool Initialize() override;
		virtual int GetCount() override;
		
		virtual int Read(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
		virtual bool Close(PFILE file) override;
		virtual PFILE Open(const char* FileName, const char* mode) override;
		virtual size_t Write(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
};

PFILE MemoryResourceAdaptor::Open(cons char* fileName, const char *mode){
	Module* pModule=SkyModuleManager::GetInstance()->FindModule(fileName);//모듈매니저를 이용하여 모듈 구조체 찾음 
	if(pModule){
		PFILE file=new FILE;
		file->_deviceID='L';
		strcpy(file->_name, fileName);
		file->_id=(DWORD)pModule;//모듈구조체 저장 
		file->_fileLength=pModule->ModuleEnd-pModule->ModuleStart;
		file->_eof=0;
		file->_position=0;
		return file;
	}
	return nullptr;
}

int MemoryResourceAdaptor::Read(PFILE file, unsigned char* buffer, unsigned int size, int count){
	Module* pModule=(Module*)file->_id;
	int remain=file->_fileLength-file->_position;//남아있는 데이터크기 
	int readCount=size*count;//읽어야하는 데이터크기
	if(remaine<readCount){
		readCount=remain;
		file->_eof=1;
	} 
	
	memcpy(buffer, ((char*)pModule->ModuleStart)+file->_position, readCount);//현재 포지션부터 readCount만큼 쓴다
	file->_position+=readCount;
	return readCount; 
}
