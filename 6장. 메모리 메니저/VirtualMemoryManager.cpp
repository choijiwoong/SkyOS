//����޸𸮸޴���
namespace VirtualMemoryManager{
	bool Initialize();//������ ���丮 ���� �� PDBR���, ����¡ Ȱ��ȭ 
	
	bool AllocPage(PTE* e);
	void FreePage(PTR* e);
	
	PageDirectory* CreateCommonPageDirectory();
	void SetPageDirectory(PageDirectory* dir);
	
	//PDBR�������Ϳ� ����
	bool SetCurPageDirectory(PageDirectory* dir);
	bool SetKernelPageDirectory(PageDirectory* dir); 
	
	//���� ������ ���丮�� �����´�(PDBR����) 
	PageDirectory* GetCurPageDirectory();
	PageDirectory* GetKernelPageDirectory();
	
	void ClearPageTable(PageTable* p);//���� ������ ���̺� �ʱ�ȭ 
	PTE* GetPTE(PageTable* p, uint32_t addr);//������ ������ ���̺��� �ּҿ� �ش�Ǵ� ���������̺�Ʈ���� �����´�. 
	//�ּҿ��� PTE��ŭ�� �д´� (?!)
	uint32_t GetPageTableEntryIndex(uint32_t addr);
	uint32_t GetPageTableIndex(uint32_t addr);
	
	void ClearPageDirectory(PageDirectory* dir);//���� ������ ���丮 �ʱ�ȭ 
	PDE* GetPDE(PageDirectory* p, uint32_t addr);
	
	bool CreatePageTable(PageDirectory* dir, uint32_t virt, uint32_t flags);//PDT�� virt��°�� PET�����. 
	void MapPhysicalAddressToVirtualAddress(PageDirectory* directory, uint32_t virt, uint32_t phys, uint32_t flags);//PDT�� virt->phys�� �����Ѵ�. 
	void* GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress);//���� �����ּҰ��� ��´�. 
	
	void UnmapPageTable(PageDirectory* dir, uint32_t virt);//PET�� �����Ѵ�. 
	void UnmapPhysicalAddress(PageDirectory* dir, uint32_t virt);
	
	void FreePageDirectory(PageDirectory* dir);//PDT���� 
	PageDirectory* CreatePageDirectory();//PDT���� 
} 

bool VirtualMemoryManager::Initialize(){
	SkyConsole::Print("Virtual Memory Manager Init..\n");
	
	for(int i=0; i<MAX_PAGE_DIRECTORY_COUNT; i++){//�������� ������ ���丮 Ǯ�� ����(PDT���ƴ� PDP�� ����ϴ°�!)
		g_pageDirectoryPool[i]=(PageDirectory*)PhysicalMemoryManager::AllocBlock();//�����޸𸮸Ŵ������� �Ҵ��� ����� ������ ���丮�� ���. 
		g_pageDirectoryAvailable[i]=true;//�Ҵ��߱⿡ ��밡������ ǥ�� 
	}
	PageDirectory* dir=CreateCommonPageDirectory();//������ ���丮 ���� ����. 
	if(dir==nullptr)
		return false;
		
	//������ ���丮�� PDBR�������Ϳ� �ε� 
	SetCurPageDirectory(dir);
	SetKernelPageDirectory(dir);
	SetPageDirectory(dir);
	
	//����¡ Ȱ��ȭ(PDBR�� ������ PD�� ����)
	PhysicalMemoryManager::EnablePaging(true);
	return true;
}

PageDirectory* VirtualMemoryManager::CreateCommonPageDirectory(){//������ ���丮 ���� 
	//1024���� ������ ���̺��� ������ 1024���� ������ ��Ʈ���� ������ ������(���)�� 4KB�̱⿡ 1024*1024*4KB=4GBǥ���� �����ϴ�. 
	int index=0;
	for(; index<MAX_PAGE_DIRECTORY_COUNT; index++){//��밡���� PDE�� ã�´�. 
		if(g_pageDirectoryAvailable[index]==true)
			break;
	}
	if(index==MAX_PAGE_DIRECTORY_COUNT)//��밡���� PDE�� ���ٸ� 
		return nullptr;
	
	PageDirectory* dir=g_pageDirectoryPool[index];
	g_pageDirectoryAvailable[index]=false;//��������� ǥ�� 
	memset(dir, 0, sizeof(PageDirectory));//�ʱ�ȭ
	
	uint32_t frame=0x00000000;//�����ּ� ���� ��巹�� 
	uint32_t virt=0x00000000; //�����ּ� ���� ��巹��
	for(int i=0; i<2; i++){//PDE�� PD���� 4MB�ּ� ������ ǥ���ϴ� ������ ���̺�PET�� ����. �����ּ�==�����ּ��� ���̵�ƼƼ ������ ���̺��� �����غ���. 
		PageTable* identityPageTable=(PageTable*)PhysicalMemoryManager::AllocBlock();
		if(identityPageTable==NULL)
			return nullptr;
		
		memset(identityPageTable, 0, sizeof(PageTable));
		
		//�����ּҸ� �����ּҿ� �����ϰ� ����
		for(int j=0; j<PAGES_PER_TABLE; j++, frame+=PAGE_SIZE, virt+=PAGE_SIZE){ //PAGES_PER_TABLE�� 1024��. PAGE_SIZE�� �������� ũ��� 4KB 
			PTE page=0;//���������̺�Ʈ�� ���� 
			PageTableEntry::AddAttribute(&page, I86_PTE_PRESENT);//PTE�Ӽ� �ο� 
			PageTableEntry::SetFrame(&page, frame);//��Ʈ�� ������ �����ּҸ� ���� 
			identityPageTable->m_entries[PAGE_TABLE_INDEX(virt)]=page;//�����޸𸮸� ����Ű�� ���������̺��� ��Ʈ���� ���� �������� �����ϸ�, �ε��������� ����޸��ּҸ� ���. ��, PET[virt]=page(frame)�� ����Ű���� PET�� �ʱ�ȭ ����. 
			//������ ���̺��� ���� ��Ʈ��PTE�� �������� �����ϴ� ����. m_entries�� ����! ��ũ�� �Լ��� �����ּҿ��� PET�� �ε����� ����. 
		}
		
		//���� PET�� PDE�� ����.
		PDE* identityEntry=&dir->m_entries[PAGE_DIRECTORY_INDEX((virt-0x00400000))];//�����ּҿ� �ش��ϴ� ������ ���丮�� ��Ʈ���� �����´�.  (PAGE_DIRECTORY_INDEX�� �����ּҿ��� PTE�� �ε����� �˾Ƴ��� ��ũ���Լ�) 
		PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_PRESENT | I86_PDE_WRITABLE);//PDE�Ӽ��� ������� �ο� 
		PageDirectoryEntry::SetFrame(identityEntry, (uint32_t)identityPageTable);//�����ص� PageTable�� PDE�� ����. 
	} 
	return dir;//������ ���丮 Ǯ�� ����. 
}
