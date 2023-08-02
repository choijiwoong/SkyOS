/*
Virtual File System�� Ŀ���ڵ忡�� �ϵ��ũ�� �����ϴ� �������̽��̴�. Ŀ�� <-> VFS <-> File System <-> ������ġ  
  File System�� FAT(File Allocation Table)�� 512����Ʈ ���͵��� ������ Ŭ�����Ͷ�� ������ ������ �����Ѵ�.
Ŭ�������� �ε�����ȣ, �����̸�, �����ð��� ������ ����ü ��Ʈ��(4byte)�� �迭�� ���丮�� FAT���˽� �ڵ����� �����ȴ�. 
0,1��Ʈ���� FAT12 16 32�� �����ϱ� ���� ����ϸ�, END��Ʈ�� �̿��� ���� �����ϸ� �ϳ��� �����͸� ���� Ŭ�����Ͱ� ���������� �����Ѵ�. 
*/
#define STORAGE_DEVICE_MAX 10

class StorageManager{
	public:
		~StorageManager();
		static StorageManager* GetInstance(){//singleton
			if(m_pStorageManager == nullptr)
				m_pStorageManager=new StorageManager();
			return m_pStorageManager;
		}
		
		//interface
		bool RegisterFileSystem(FileSysAdaptor* fsys, DWORD deviceID);//���� �ý��� ���(�÷��� FAT16, �ϵ� FAT32) 
		bool UnregisterFileSystem(FileSysAdaptor* fsys);
		bool UnregisterFileSystemByID(DWORD deviceID);
		
		bool SetCurrentFileSystemByID(DWORD deviceID);//����ϴ� ����̽� ����? 
		bool SetCurrentFileSystem(FileSysAdaptor* fsys);
		
		//file method
		PFILE OpenFile(const char* fname, const char *mode);//��ϵ� ���� �ý����� �̿��Ͽ� ���� ���� 
		int ReadFile(PFILE file, unsigned char* Buffer, unsigned int size, int count);
		int WriteFile(PFILE file, unsigned char* Buffer, unsigned int size, int count);
		bool CloseFile(PFILE file);
		
	private:
		StorageManager();//������ ����! �̰� ����Ƽ�� �ڹٿ��� ����!! 
		static StorageManager* m_fileSystems[STOROAGE_DEVICE_MAX];
		
		FileSysAdaptor* m_fileSystems[STORAGE_DEVICE_MAX];//���丮?_Ŭ������ ���� ������ ��ƼƼ �迭? 
		int m_storageCount;
		FileSysAdaptor* m_pCurrentFileSystem;//���� �ý��� ���� 
}; 

//ǥ���������̽� ������ ���� ����������Լ� ������
FILE *fopen(const char* filename, const char* mode){
	return StorageManager::GetInstance()->OpenFile(filename, mode);
}
size_t fread(void *ptr, size_t size, size_t count, FILE* stream){
	return StorageManager::GetInstance()->ReadFile(stream, (unsigned char*)ptr, size, count);
}
size_t fwrite(cosnt void* ptr, size_t size, size_t count, FILE* stream){
	return StorageManager::GetInstance()->WriteFile(stream, (unsigned char*)ptr, size, count);
}
int fclose(FILE* stream){
	return StorageManager::GetInstance()->CloseFile(stream);
}
int feof(FILE* stream){
	if(stream->_eod!=0)
		return stream->_eof;
	return 0;
}
int fseek(FILE* stream, long int offset, int whence){
	if(SEEK_CUR==whence){//ã�����ϴ� �� ���� �а� �ִ� ��ġ��� �״�� �б� 
		fgetc(stream);
		return 1;
	}
	...
	return 0;
}
long int ftell(FILE* stream){
	return (long int)stream->_position;//���� ��ġ ��ȯ 
}
int fgetc(FILE* stream){
	char buf[2];//\0������ 2����Ʈ�ΰ�? 
	
	int readCount=StorageManager::GetInstance()->ReadFile(stream, (unsigned char*)buf, 1, 1);//1ũ�⸸ŭ 1�� �о�� 
	if(readCount==0)
		return EOF;
	return buf[0];
}
char* fgetc(char *dst, int max, FILE *fp){//dst�������� max��ŭ �о�� 
	int c=0;
	char* p=nullptr;
	for(p=dst, max--; max>0; max--){
		if((c=fgetc(fp))==EOF)//\0
			break;
		if(c==0x0d)//\r
			continue;
		
		*p++=c;//�б� 
		if(c==0x0a)//\n
			break; 
	}
	*p=0;
	if(p==dst || c==EOF)//������ ���ų�, ������ ������ ��� 
		return NULL;
	return (dst);
}
