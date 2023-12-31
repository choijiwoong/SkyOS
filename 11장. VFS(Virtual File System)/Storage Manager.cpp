/*
Virtual File System은 커널코드에서 하드디스크에 접근하는 인터페이스이다. 커널 <-> VFS <-> File System <-> 저장장치  
  File System중 FAT(File Allocation Table)은 512바이트 섹터들의 집합인 클러스터라는 단위로 파일을 관리한다.
클러스터의 인덱스번호, 파일이름, 생성시각을 가지는 구조체 엔트리(4byte)의 배열인 디렉토리가 FAT포맷시 자동으로 생성된다. 
0,1엔트리는 FAT12 16 32를 구분하기 위해 사용하며, END비트를 이용해 끝을 구분하며 하나의 데이터를 여러 클러스터가 연쇄적으로 저장한다. 
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
		bool RegisterFileSystem(FileSysAdaptor* fsys, DWORD deviceID);//파일 시스템 등록(플로피 FAT16, 하드 FAT32) 
		bool UnregisterFileSystem(FileSysAdaptor* fsys);
		bool UnregisterFileSystemByID(DWORD deviceID);
		
		bool SetCurrentFileSystemByID(DWORD deviceID);//사용하는 디바이스 지정? 
		bool SetCurrentFileSystem(FileSysAdaptor* fsys);
		
		//file method
		PFILE OpenFile(const char* fname, const char *mode);//등록된 파일 시스템을 이용하여 파일 조작 
		int ReadFile(PFILE file, unsigned char* Buffer, unsigned int size, int count);
		int WriteFile(PFILE file, unsigned char* Buffer, unsigned int size, int count);
		bool CloseFile(PFILE file);
		
	private:
		StorageManager();//생성자 금지! 이거 이펙티브 자바에서 본거!! 
		static StorageManager* m_fileSystems[STOROAGE_DEVICE_MAX];
		
		FileSysAdaptor* m_fileSystems[STORAGE_DEVICE_MAX];//디렉토리?_클러스터 정보 가지는 엔티티 배열? 
		int m_storageCount;
		FileSysAdaptor* m_pCurrentFileSystem;//파일 시스템 저장 
}; 

//표준인터페이스 제공을 위한 파일입출력함수 재정의
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
	if(SEEK_CUR==whence){//찾고자하는 게 지금 읽고 있는 위치라면 그대로 읽기 
		fgetc(stream);
		return 1;
	}
	...
	return 0;
}
long int ftell(FILE* stream){
	return (long int)stream->_position;//현재 위치 반환 
}
int fgetc(FILE* stream){
	char buf[2];//\0때문에 2바이트인가? 
	
	int readCount=StorageManager::GetInstance()->ReadFile(stream, (unsigned char*)buf, 1, 1);//1크기만큼 1번 읽어라 
	if(readCount==0)
		return EOF;
	return buf[0];
}
char* fgetc(char *dst, int max, FILE *fp){//dst에서부터 max만큼 읽어라 
	int c=0;
	char* p=nullptr;
	for(p=dst, max--; max>0; max--){
		if((c=fgetc(fp))==EOF)//\0
			break;
		if(c==0x0d)//\r
			continue;
		
		*p++=c;//읽기 
		if(c==0x0a)//\n
			break; 
	}
	*p=0;
	if(p==dst || c==EOF)//변함이 없거나, 읽은게 종료일 경우 
		return NULL;
	return (dst);
}
