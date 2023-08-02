//����ũ�� ��Ű���޴����� �ʿ��� ������ Ŀ�ο� �����ϴ� ��Ȱ�� GRUB�ܿ��� �ڵ��ε��� �� �ֵ��� �Ŵ����� ������ ó���� �����ϴ�. 
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
	Module* pModule=SkyModuleManager::GetInstance()->FindModule(fileName);//���Ŵ����� �̿��Ͽ� ��� ����ü ã�� 
	if(pModule){
		PFILE file=new FILE;
		file->_deviceID='L';
		strcpy(file->_name, fileName);
		file->_id=(DWORD)pModule;//��ⱸ��ü ���� 
		file->_fileLength=pModule->ModuleEnd-pModule->ModuleStart;
		file->_eof=0;
		file->_position=0;
		return file;
	}
	return nullptr;
}

int MemoryResourceAdaptor::Read(PFILE file, unsigned char* buffer, unsigned int size, int count){
	Module* pModule=(Module*)file->_id;
	int remain=file->_fileLength-file->_position;//�����ִ� ������ũ�� 
	int readCount=size*count;//�о���ϴ� ������ũ��
	if(remaine<readCount){
		readCount=remain;
		file->_eof=1;
	} 
	
	memcpy(buffer, ((char*)pModule->ModuleStart)+file->_position, readCount);//���� �����Ǻ��� readCount��ŭ ����
	file->_position+=readCount;
	return readCount; 
}
