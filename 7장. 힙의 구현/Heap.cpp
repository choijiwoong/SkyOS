//블록의 헤더(9Byte) 
typedef struct{
	u32int magic;//heap currption의 checksum값 
	u8int is_hole;//홀인지 블록인지(사용안하고 있는 영역인지 사용하고있는 영역인지) 
	u32int size;//헤더+사이즈+푸터=블록의 크기 
} header_t;
//블록의 푸터(8Byte)
typedef struct{
	u32int magic;//buffer overrun의 checksum값 
	header_t *header;
}; footer_t 
//고로 할당받고자하는 공간의 크기보다 17Byte만큼 큰 공간이 필요하다.(이 구조체의 설계대로라면) 


//****힙의 생성****
bool HeapManager::InitKernelHeap(int heapFrameCount){//아마 블록포인터 디스크립터 테이블인 힙 인덱스 테이블의 크기이려나? 
	PageDirectory* curPageDirectory=GetCurPageDirectory();//PDT를 가져온다(메모리 공간에 접근하기 위한) 
	
	void* pVirtualHeap=(void*)(KERNEL_VIRTUAL_HEAP_ADDRESS);//힙의 가상주소
	m_heapFrameCount=heapFrameCount;//Heap Index Table의 크기 
	
	m_pKernelHeapPhysicalMemory=PhysicalMemoryManager::AllocBlocks(m_heapFrameCount);//물리 메모리 할당, HIT생성. 
	//32bit 컴퓨터에서 힙의 페이지수가 12800개라면, 연속적인 메모리 12800*블록1개크기(4byte_32bit)만큼의 공간이 할당되어 50MB를 가진다. 
	if(m_pKernelHeapPhysicalMemory==NULL){
		#ifdef _HEAP_DEBUG
			SkyConsole::Print("kernel heap allocation fail. frame count: %d\n", m_heapFrameCount);
		#endif
			return false;
	} 
	
	#ifdef _HEAP_DEBUG
		SkyConsole::Print("kernel heap allocation success. frame count : %d\n", m_heapFrameCount);
	#endif
		//힙의 마지막 주소
		int virtualEndAddress=(uint32_t)pVirtualHeap+m_heapFrameCount*PMM_BLOCK_SIZE;//블록사이즈*HIT크기만큼을 더해 끝나는 주소를 얻는다. 
		MapHeapToAddressSpace(curPageDirectory);//할당된 Heap의 시작주소, 크기, 끝주소를 이용하여 PDT에 등록한다(페이징, 가상->물리) 
		
		create_kernel_heap((u32int)pVirtualHeap, (uint32_t)virtualEndAddress, (uint32_t)virtualEndAddress, 0, 0);//인자가 무엇을 의미하는지.. 
		return true; 
} 

bool HeapManager::MapHeapToAddressSpace(PageDirectory* curPageDirectory){//KERNAL_VIRTUAL_HEAP_ADDRESS를 m_pKernelHeapPhysicalMemory에 PAGE_SIZE간격별로 m_heapFrameCount만큼 매핑(차피 연속적인 공간을 할당받았기에) 
	int endAddress=(uint32_t)KERNEL_VIRTUAL_HEAP_ADDRESS+m_heapFrameCount*PMM_BLOCK_SIZE;
	for(int i=0; i<m_heapFrameCount; i++)//한번에 페이지 크기(4KB)만큼 매핑되기에 프레임 수 만큼 반복을 하여 전체 50MB 페이징 매핑을 완료한다. 
		MapPhysicalAddressToVirtualAddress(curPageDirectory, (uint32_t)KERNEL_VIRTUAL_HEAP_ADDRESS + i * PAGE_SIZE, (uint32_t)m_pKernelHeapPhysicalMemory + i * PAGE_SIZE, II86_PTE_PRESENT | I86_PTE_WRITABLE);
	return true;
}


//****커널 힙 인터페이스****
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x123890AB
#define HEAP_VIRTUAL_HEAP_ADDRESS 0x10000000 

heap_t *create_kernel_heap(u32int start, u32int end, u32int max, u8int supervisor, u8int readonly); //커널 힙 생성 
heap_t *create_heap(u32int start, u32int end, u32int max, u8int supervisor, u8int readonly); //프로세스 힙 생성 

void *alloc(u32int size, u8int page_align, heap_t *heap);//메모리 할당. 힙을 이용한 동적! 
void free(void *p, heap_t *heap);//메모리 해제. 힙을 이용한 동적! 
u32int kmalloc(u32int sz);//동기화적용된 alloc, 내부적으로 kmalloc_int(내부적으로 alloc을 호출)를 호출 
void kfree(void *p); //동기화 적용된 free, 내부적으로 free를 호출 

u32int kmalloc_int(u32int sz, int align, u32int* phys){
	void *addr=alloc(sz, (u8int)align, &kheap);//힙 시스템에서 홀의 주소를 리턴(힙에서 사용가능한 적절한 사이즈의 홀주소) 
	return (u32int)addr;
} 
void kfree(void *p){//힙 시스템(HIT)에 회수하려는 메모리를 넣는다. 이때 인접한 블록이 홀이면 병합한다. 
	EnterCriticalSection();//할당을 요청하면서 컨텍스트 스위칭으로 해제를 수행하면 문제가 될 수 있기에 
	free(p, &kheap);
	LeaveCriticalSection();
}
