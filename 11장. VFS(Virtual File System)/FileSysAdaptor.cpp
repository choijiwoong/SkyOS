//StorageManager가 FloppyDiskAdaptor, HDDAdaptor, RamDiskAdaptor등에 공통적으로 접근하기위한 인터페이스를 FileSysAdaptor가 제공한다.(각기 다른 FS를 통일화) 
#define MAXPATH 10

class FileSysAdaptor{
	public:
		FileSysAdaptor(char* deviceName, DWORD deviceID);
		~FileSysAdaptor();
		
		virtual FILE* Open(const char* FileName, const char *mode)=0;//pure
		virtual int Read(PFILE file, unsigned char* buffer, unsigned int size, int count)=0;
		virtual size_t Write(PFILE file, unsigned char* buffer, unsigned int size, int count)=0;
		virtual bool Close(PFILE file)=0;
		
		virtual bool Initialize()=0;
		virtual int GetCount()=0;
		
		char m_deviceName[MAXPATH];
		DWORD m_deviceID;
};
