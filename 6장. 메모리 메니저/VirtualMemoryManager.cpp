//가상메모리메니저
namespace VirtualMemoryManager{
	bool Initialize();//페이지 디렉토리 생성 및 PDBR등록, 페이징 활성화 
	
	bool AllocPage(PTE* e);
	void FreePage(PTR* e);
	
	PageDirectory* CreateCommonPageDirectory();
	void SetPageDirectory(PageDirectory* dir);
	
	//PDBR레지스터에 설정
	bool SetCurPageDirectory(PageDirectory* dir);
	bool SetKernelPageDirectory(PageDirectory* dir); 
	
	//현재 페이지 디렉토리를 가져온다(PDBR에서) 
	PageDirectory* GetCurPageDirectory();
	PageDirectory* GetKernelPageDirectory();
	
	void ClearPageTable(PageTable* p);//받은 페이지 테이블 초기화 
	PTE* GetPTE(PageTable* p, uint32_t addr);//지정된 페이지 테이블에서 주소에 해당되는 페이지테이블엔트리를 가져온다. 
	//주소에서 PTE만큼을 읽는다 (?!)
	uint32_t GetPageTableEntryIndex(uint32_t addr);
	uint32_t GetPageTableIndex(uint32_t addr);
	
	void ClearPageDirectory(PageDirectory* dir);//받은 페이지 디렉토리 초기화 
	PDE* GetPDE(PageDirectory* p, uint32_t addr);
	
	bool CreatePageTable(PageDirectory* dir, uint32_t virt, uint32_t flags);//PDT에 virt번째에 PET만든다. 
	void MapPhysicalAddressToVirtualAddress(PageDirectory* directory, uint32_t virt, uint32_t phys, uint32_t flags);//PDT에 virt->phys를 세팅한다. 
	void* GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress);//실제 물리주소값을 얻는다. 
	
	void UnmapPageTable(PageDirectory* dir, uint32_t virt);//PET를 해제한다. 
	void UnmapPhysicalAddress(PageDirectory* dir, uint32_t virt);
	
	void FreePageDirectory(PageDirectory* dir);//PDT삭제 
	PageDirectory* CreatePageDirectory();//PDT생성 
} 

bool VirtualMemoryManager::Initialize(){
	SkyConsole::Print("Virtual Memory Manager Init..\n");
	
	for(int i=0; i<MAX_PAGE_DIRECTORY_COUNT; i++){//여러개의 페이지 디렉토리 풀을 생성(PDT가아닌 PDP로 사용하는고만!)
		g_pageDirectoryPool[i]=(PageDirectory*)PhysicalMemoryManager::AllocBlock();//물리메모리매니저에서 할당한 블록을 페이지 디렉토리로 사용. 
		g_pageDirectoryAvailable[i]=true;//할당했기에 사용가능함을 표시 
	}
	PageDirectory* dir=CreateCommonPageDirectory();//페이지 디렉토리 실제 생성. 
	if(dir==nullptr)
		return false;
		
	//페이지 디렉토리를 PDBR레지스터에 로드 
	SetCurPageDirectory(dir);
	SetKernelPageDirectory(dir);
	SetPageDirectory(dir);
	
	//페이징 활성화(PDBR에 설정한 PD로 연결)
	PhysicalMemoryManager::EnablePaging(true);
	return true;
}

PageDirectory* VirtualMemoryManager::CreateCommonPageDirectory(){//페이지 디렉토리 생성 
	//1024개의 페이지 테이블을 가지며 1024개의 페이지 엔트리를 가지고 프레임(블록)이 4KB이기에 1024*1024*4KB=4GB표현이 가능하다. 
	int index=0;
	for(; index<MAX_PAGE_DIRECTORY_COUNT; index++){//사용가능한 PDE를 찾는다. 
		if(g_pageDirectoryAvailable[index]==true)
			break;
	}
	if(index==MAX_PAGE_DIRECTORY_COUNT)//사용가능한 PDE가 없다면 
		return nullptr;
	
	PageDirectory* dir=g_pageDirectoryPool[index];
	g_pageDirectoryAvailable[index]=false;//사용중임을 표시 
	memset(dir, 0, sizeof(PageDirectory));//초기화
	
	uint32_t frame=0x00000000;//물리주소 시작 어드레스 
	uint32_t virt=0x00000000; //가상주소 시작 어드레스
	for(int i=0; i<2; i++){//PDE에 PD각각 4MB주소 영역을 표현하는 페이지 테이블PET을 생성. 가상주소==물리주소인 아이덴티티 페이지 테이블을 구현해본다. 
		PageTable* identityPageTable=(PageTable*)PhysicalMemoryManager::AllocBlock();
		if(identityPageTable==NULL)
			return nullptr;
		
		memset(identityPageTable, 0, sizeof(PageTable));
		
		//물리주소를 가상주소와 동일하게 매핑
		for(int j=0; j<PAGES_PER_TABLE; j++, frame+=PAGE_SIZE, virt+=PAGE_SIZE){ //PAGES_PER_TABLE은 1024개. PAGE_SIZE는 페이지의 크기로 4KB 
			PTE page=0;//페이지테이블엔트리 생성 
			PageTableEntry::AddAttribute(&page, I86_PTE_PRESENT);//PTE속성 부여 
			PageTableEntry::SetFrame(&page, frame);//엔트리 값으로 물리주소를 설정 
			identityPageTable->m_entries[PAGE_TABLE_INDEX(virt)]=page;//물리메모리를 가리키는 페이지테이블의 엔트리로 위의 페이지를 설정하며, 인덱스값으로 가상메모리주소를 사용. 즉, PET[virt]=page(frame)을 가리키도록 PET를 초기화 설정. 
			//페이지 테이블의 측정 엔트리PTE에 페이지를 설정하는 예시. m_entries로 접근! 매크로 함수로 선형주소에서 PET의 인덱스를 추출. 
		}
		
		//위의 PET를 PDE에 세팅.
		PDE* identityEntry=&dir->m_entries[PAGE_DIRECTORY_INDEX((virt-0x00400000))];//가상주소에 해당하는 페이지 디렉토리의 엔트리를 가져온다.  (PAGE_DIRECTORY_INDEX는 선형주소에서 PTE의 인덱스를 알아내는 매크로함수) 
		PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_PRESENT | I86_PDE_WRITABLE);//PDE속성과 쓰기권한 부여 
		PageDirectoryEntry::SetFrame(identityEntry, (uint32_t)identityPageTable);//세팅해둔 PageTable을 PDE에 매핑. 
	} 
	return dir;//페이지 디렉토리 풀을 리턴. 
}
