//����� ���(9Byte) 
typedef struct{
	u32int magic;//heap currption�� checksum�� 
	u8int is_hole;//Ȧ���� �������(�����ϰ� �ִ� �������� ����ϰ��ִ� ��������) 
	u32int size;//���+������+Ǫ��=����� ũ�� 
} header_t;
//����� Ǫ��(8Byte)
typedef struct{
	u32int magic;//buffer overrun�� checksum�� 
	header_t *header;
}; footer_t 
//��� �Ҵ�ް����ϴ� ������ ũ�⺸�� 17Byte��ŭ ū ������ �ʿ��ϴ�.(�� ����ü�� �����ζ��) 


//****���� ����****
bool HeapManager::InitKernelHeap(int heapFrameCount){//�Ƹ� ��������� ��ũ���� ���̺��� �� �ε��� ���̺��� ũ���̷���? 
	PageDirectory* curPageDirectory=GetCurPageDirectory();//PDT�� �����´�(�޸� ������ �����ϱ� ����) 
	
	void* pVirtualHeap=(void*)(KERNEL_VIRTUAL_HEAP_ADDRESS);//���� �����ּ�
	m_heapFrameCount=heapFrameCount;//Heap Index Table�� ũ�� 
	
	m_pKernelHeapPhysicalMemory=PhysicalMemoryManager::AllocBlocks(m_heapFrameCount);//���� �޸� �Ҵ�, HIT����. 
	//32bit ��ǻ�Ϳ��� ���� ���������� 12800�����, �������� �޸� 12800*���1��ũ��(4byte_32bit)��ŭ�� ������ �Ҵ�Ǿ� 50MB�� ������. 
	if(m_pKernelHeapPhysicalMemory==NULL){
		#ifdef _HEAP_DEBUG
			SkyConsole::Print("kernel heap allocation fail. frame count: %d\n", m_heapFrameCount);
		#endif
			return false;
	} 
	
	#ifdef _HEAP_DEBUG
		SkyConsole::Print("kernel heap allocation success. frame count : %d\n", m_heapFrameCount);
	#endif
		//���� ������ �ּ�
		int virtualEndAddress=(uint32_t)pVirtualHeap+m_heapFrameCount*PMM_BLOCK_SIZE;//��ϻ�����*HITũ�⸸ŭ�� ���� ������ �ּҸ� ��´�. 
		MapHeapToAddressSpace(curPageDirectory);//�Ҵ�� Heap�� �����ּ�, ũ��, ���ּҸ� �̿��Ͽ� PDT�� ����Ѵ�(����¡, ����->����) 
		
		create_kernel_heap((u32int)pVirtualHeap, (uint32_t)virtualEndAddress, (uint32_t)virtualEndAddress, 0, 0);//���ڰ� ������ �ǹ��ϴ���.. 
		return true; 
} 

bool HeapManager::MapHeapToAddressSpace(PageDirectory* curPageDirectory){//KERNAL_VIRTUAL_HEAP_ADDRESS�� m_pKernelHeapPhysicalMemory�� PAGE_SIZE���ݺ��� m_heapFrameCount��ŭ ����(���� �������� ������ �Ҵ�޾ұ⿡) 
	int endAddress=(uint32_t)KERNEL_VIRTUAL_HEAP_ADDRESS+m_heapFrameCount*PMM_BLOCK_SIZE;
	for(int i=0; i<m_heapFrameCount; i++)//�ѹ��� ������ ũ��(4KB)��ŭ ���εǱ⿡ ������ �� ��ŭ �ݺ��� �Ͽ� ��ü 50MB ����¡ ������ �Ϸ��Ѵ�. 
		MapPhysicalAddressToVirtualAddress(curPageDirectory, (uint32_t)KERNEL_VIRTUAL_HEAP_ADDRESS + i * PAGE_SIZE, (uint32_t)m_pKernelHeapPhysicalMemory + i * PAGE_SIZE, II86_PTE_PRESENT | I86_PTE_WRITABLE);
	return true;
}


//****Ŀ�� �� �������̽�****
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x123890AB
#define HEAP_VIRTUAL_HEAP_ADDRESS 0x10000000 

heap_t *create_kernel_heap(u32int start, u32int end, u32int max, u8int supervisor, u8int readonly); //Ŀ�� �� ���� 
heap_t *create_heap(u32int start, u32int end, u32int max, u8int supervisor, u8int readonly); //���μ��� �� ���� 

void *alloc(u32int size, u8int page_align, heap_t *heap);//�޸� �Ҵ�. ���� �̿��� ����! 
void free(void *p, heap_t *heap);//�޸� ����. ���� �̿��� ����! 
u32int kmalloc(u32int sz);//����ȭ����� alloc, ���������� kmalloc_int(���������� alloc�� ȣ��)�� ȣ�� 
void kfree(void *p); //����ȭ ����� free, ���������� free�� ȣ�� 

u32int kmalloc_int(u32int sz, int align, u32int* phys){
	void *addr=alloc(sz, (u8int)align, &kheap);//�� �ý��ۿ��� Ȧ�� �ּҸ� ����(������ ��밡���� ������ �������� Ȧ�ּ�) 
	return (u32int)addr;
} 
void kfree(void *p){//�� �ý���(HIT)�� ȸ���Ϸ��� �޸𸮸� �ִ´�. �̶� ������ ����� Ȧ�̸� �����Ѵ�. 
	EnterCriticalSection();//�Ҵ��� ��û�ϸ鼭 ���ؽ�Ʈ ����Ī���� ������ �����ϸ� ������ �� �� �ֱ⿡ 
	free(p, &kheap);
	LeaveCriticalSection();
}
