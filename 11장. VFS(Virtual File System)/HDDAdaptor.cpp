typedef struct _FILE{//엔티티 구조체 
	char _name[32];
	DWORD _flags;
	DWORD _fileLength;
	DWORD _id;
	DWORD _eof;
	DWORD _position;
	DWORD _currentCluster;
	DWORD _deviceID;
}FILE, *PFILE;

//실제 저장장치의 접근을 위해 FileSysAdaptor인터페이스를 구현한다. 커널 코드와 디바이스 코드를 분리하는 이를 어댑터 패턴이라고 한다. 
class HDDAdaptor: public FileSysAdaptor{
	public:
		HDDAdaptor(char* deviceName, DWORD deviceID);
		virtual ~HDDAdaptor();
		
		void PrintHDDInfo();
		
		bool Initialize() override;
		virtual int GetCount() override;
		virtual int Read(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
		virtual bool Close(PFILE file) override;
		virtual PFILE Open(const char* FileName, const char *mode) override;
		virtual size_t Write(PFILE file, unsigned char* buffer, unsigned int size, int count) override;
};

PFILE HDDAdaptor::Open(const char* fileName, const char* mode){
	std::string name="C:";
	name+=fileName;
	
	int handle=FATFileOpen('C', (char*)name.c_str(), 0);//파일이 존재하면 파일핸들을 가져온다.
	if(handle==0){
		SkyConsole::Print("File Open Fail: %s\n", fileName);
		return nullptr;
	} 
	
	PFILE file=new FILE;//HDD내용을 공통포맷(구조체)로 변환하여 리턴
	file->_deviceID='C';
	strcpy(file->name, fileName) ;
	file->_id=handle;
	
	return file;
}
