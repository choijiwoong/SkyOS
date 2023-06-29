//���� �޸� �Ŵ��� �������̽�. ������==���==������ �ǹ��̴�. 
namespace PhysicalMemoryManager{//�̱��ϴ����� ���� namespace��� 
	void Initialize(multiboot_info* bootinfo);
	
	void SetBit(int bit);
	void UnsetBit(int bit);
	uint32_t GetMemoryMapSize();
	uint32_t GetKernelEnd();
	
	void* AllocBlock();
	void FreeBlock(void*);
	void* AllocBlocks(size_t);
	void FreeBlocks(void*, size_t);
	
	size_t GetMemorySize();
	
	unsigned int GetFreeFrame();//��밡���� ������(���) ��ȣ�� ��´� 
	unsigned int GetFreeFrames(size_t size);//���ӵ� �������� ���� ������(���) ��ȣ�� ��´� 
	
	uint32_t GetUsedBlockCount();
	uint32_t GetFreeBlockCount();
	
	uint32_t GetFreewMemory();
	
	uint32_t GetTotalBlockCount();
	uint32_t GetBlockSize();//4KB
}

//Ư�� ����� ���ǰ� �ִ����� ��Ÿ���� ��Ʈ�� �迭�� char* m_pMemoryMap���� ��Ÿ����. ���ظ� ���� char*�� ����Ʈ �迭�� ��Ÿ������, �����δ� 4����Ʈ�� �迭�̴� .(32bit) 
//��Ʈ�� �迭���� ��Ʈ�� �ϳ��� �ϳ��� ���(�ѹ��� ó���� �� �ִ� �ּ� ���� 32bit(4KB) 16bit 64bit ��)�� ��Ÿ���� .
void PhysicalMemoryManager::Initialize(multiboot_info* bootinfo){
	SkyConsole::Print("Physical Memory Manager Init..\n");
	
	g_totalMemorySize=GetTotalMemory(bootinfo);//bootinfo���� ��ü �޸� ũ�⸦ ����
	m_usedBlocks=0;
	m_memorySize=g_totalMemorySize;//���� �޸� ������ 
	m_maxBlocks=m_memorySize / PMM_BLOCK_SIZE;//�ִ�� = �޸� ������ / ��� ������
	
	int pageCount=m_maxBlocks / PMM_BLOCKS_PER_BYTE / PAGE_SIZE; // 1����Ʈ ��Ÿ���µ� ���Ǵ� BLOCK�� ����?�� �ִ����� ������, ������ ������� ������ �������� ������ �� 
	if(pageCount==0)
		pageCount=1;
	
	//Ŀ�α��������� ������ �޸� ��Ʈ�� ���� ����
	m_pMemoryMap=(uint32_t*)GetKernelEnd(bootinfo);
	
	SkyConsole::Print("Total Memory (%dMB)\n", g_totalMemorySize / 1048576);
	SkyConsole::Print("BitMap Start Address(0x%x)\n", m_pMemoryMap);
	SkyConsole::Print("BitMap Size(0x%x)\n", pageCount*PAGE_SIZE);
	
	m_memoryMapSize=m_maxBlocks / PMM_BLOCKS_PER_BYTE;//�޸� ���� ����� ǥ���ϴµ��� ���Ǵ� ����Ʈ(���) ũ��
	m_usedBlocks=GetTotalBlockCount();//������� ��ϰ����� bootinfo���� ���� �� �ֱ⿡ ��ü�� ���� ���. 
	
	int tempMemoryMapSize=(GetMemoryMapSize() / 4096) * 4096; //����Ʈũ���, 4KB�� 4096Byte. ���� �ǹ̸� ��Ÿ���� ���ؼ� ������ ���ϴ°Ű����� ���μ����� �Ҵ��� ��Ȳ�� �ǹ��ϴ°ŷ���? �ٸ� �����ϱ� ��ü�Ҵ��� ���̰� 
	if(GetMemoryMapSize() % 4096 > 0)//4KB���� �������Բ� ���� 
		tempMemoryMapSize+=4096;
	m_memoryMapSize=tempMemoryMapSize;//4KB������ MemoryMapSize�� ����
	
	unsigned char flag=0xff;//��� �޸� ����� ������̶��(�ٽ�!!�޸� ���� ��Ȱ�� ��������� �ƴ��� bit������ ǥ��, byte������ ó��) 
	memset((char*)m_pMemoryMap, flag, m_memoryMapSize);//�޸� ���� ����Ű�� ������(����Ʈ����)�� �޸� �� ũ��������� ��������� �����Ѵ�. 
	SetAvailableMemory((uint32_t)m_pMemoryMap, m_memorySize);//��ü �޸� ����� �޸𸮸��� ���� �����Ͽ� ������� �ʴ� �޸𸮸��� ��밡���ϰ� �����Ѵ�. 
}

void* PhysicalMemoryManager::AllocBlock(){
	if(GetFreeBlockCount() <= 0)
		return NULL;
	
	int frame=GetFreeFrame();//����ϰ� ���� ���� ����� �ε����� ��´�
	if(frame==-1)
		return NULL;
		
	SetBit(frame);//�޸� ��Ʈ�ʿ� �ش� ����� ��������� ����
	
	uint32_t addr=frame*PMM_BLOCK_SIZE;//���� �Ҵ�� �޸� �ּҸ� ����(*4KB). ����� uint32_t�� unsigned int�� typedef������ 4����Ʈ�� ������� �ּҰ��� ��Ÿ���µ��� ���ȴ�. 
	m_usedBlocks++;
	
	return (void*)addrl
} 

void PhycisalMemoryManager::SetBit(int bit){
	//���� AllocBlock���� GetFreeFrame�� ������ 1301�̶�� �ش� ����� �̻�����ΰ��̴�. �޸� ��Ʈ���� 4����Ʈ�� 32��(4byte=32bit)�� ����� ǥ�������ϴ� 
	//1301/32�� ��Ʈ�� �ε����� ���, �������� 1 ��, �ι�° ��Ʈ�� ��Ʈ�ϸ� ��������� ��Ÿ�� �� �ִ�. 
	// ���� �����ؼ� �޸� ��Ʈ���� �׳� ��Ϻ� ��뿩�θ� bit�迭�� ��Ÿ���� �ʹ� Ŀ���� ������ ���Ͽ� ��Ʈ�������� ������ ���̴�. 
	m_pMemoryMapBit[bit/32]|=(1<<(bit%32));//�޸𸮸��� ������ �ε����� 1(true)�� �˸��� �ڸ����� <<�Ѵ��� or�� �ش� ��Ʈ�� set���ָ� �ȴ�. 
}
