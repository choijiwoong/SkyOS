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
