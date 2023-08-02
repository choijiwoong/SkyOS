typedef struct _FILE{//��ƼƼ ����ü 
	char _name[32];
	DWORD _flags;
	DWORD _fileLength;
	DWORD _id;
	DWORD _eof;
	DWORD _position;
	DWORD _currentCluster;
	DWORD _deviceID;
}FILE, *PFILE;

//���� ������ġ�� ������ ���� FileSysAdaptor�������̽��� �����Ѵ�. Ŀ�� �ڵ�� ����̽� �ڵ带 �и��ϴ� �̸� ����� �����̶�� �Ѵ�. 
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
	
	int handle=FATFileOpen('C', (char*)name.c_str(), 0);//������ �����ϸ� �����ڵ��� �����´�.
	if(handle==0){
		SkyConsole::Print("File Open Fail: %s\n", fileName);
		return nullptr;
	} 
	
	PFILE file=new FILE;//HDD������ ��������(����ü)�� ��ȯ�Ͽ� ����
	file->_deviceID='C';
	strcpy(file->name, fileName) ;
	file->_id=handle;
	
	return file;
}
