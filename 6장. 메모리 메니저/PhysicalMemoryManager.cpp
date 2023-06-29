//물리 메모리 매니저 인터페이스. 페이지==블록==프레임 의미이다. 
namespace PhysicalMemoryManager{//싱글턴느낌을 위해 namespace사용 
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
	
	unsigned int GetFreeFrame();//사용가능한 프레임(블록) 번호를 얻는다 
	unsigned int GetFreeFrames(size_t size);//연속된 프레임의 시작 프레임(블록) 번호를 얻는다 
	
	uint32_t GetUsedBlockCount();
	uint32_t GetFreeBlockCount();
	
	uint32_t GetFreewMemory();
	
	uint32_t GetTotalBlockCount();
	uint32_t GetBlockSize();//4KB
}

//특정 블록이 사용되고 있는지를 나타내는 비트맵 배열을 char* m_pMemoryMap으로 나타낸다. 이해를 위해 char*즉 바이트 배열로 나타냈지만, 실제로는 4바이트의 배열이다 .(32bit) 
//비트맵 배열에서 비트값 하나는 하나의 블록(한번에 처리할 수 있는 최소 단위 32bit(4KB) 16bit 64bit 등)을 나타낸다 .
void PhysicalMemoryManager::Initialize(multiboot_info* bootinfo){
	SkyConsole::Print("Physical Memory Manager Init..\n");
	
	g_totalMemorySize=GetTotalMemory(bootinfo);//bootinfo에서 전체 메모리 크기를 얻음
	m_usedBlocks=0;
	m_memorySize=g_totalMemorySize;//자유 메모리 사이즈 
	m_maxBlocks=m_memorySize / PMM_BLOCK_SIZE;//최대블럭 = 메모리 사이즈 / 블록 사이즈
	
	int pageCount=m_maxBlocks / PMM_BLOCKS_PER_BYTE / PAGE_SIZE; // 1바이트 나타내는데 사용되는 BLOCK의 개수?로 최대블록을 나누고, 페이지 사이즈로 나누어 페이지의 개수를 득 
	if(pageCount==0)
		pageCount=1;
	
	//커널구역끝나는 곳에서 메모리 비트맵 구역 설정
	m_pMemoryMap=(uint32_t*)GetKernelEnd(bootinfo);
	
	SkyConsole::Print("Total Memory (%dMB)\n", g_totalMemorySize / 1048576);
	SkyConsole::Print("BitMap Start Address(0x%x)\n", m_pMemoryMap);
	SkyConsole::Print("BitMap Size(0x%x)\n", pageCount*PAGE_SIZE);
	
	m_memoryMapSize=m_maxBlocks / PMM_BLOCKS_PER_BYTE;//메모리 맵의 블록을 표현하는데에 사용되는 바이트(블록) 크기
	m_usedBlocks=GetTotalBlockCount();//사용중인 블록개수를 bootinfo에서 읽을 수 있기에 전체만 위에 계산. 
	
	int tempMemoryMapSize=(GetMemoryMapSize() / 4096) * 4096; //바이트크기고, 4KB는 4096Byte. 뭔가 의미를 나타내기 위해서 나누고 곱하는거같은데 프로세스에 할당한 상황을 의미하는거려나? 다만 예제니까 전체할당일 뿐이고 
	if(GetMemoryMapSize() % 4096 > 0)//4KB단위 끊어지게끔 세팅 
		tempMemoryMapSize+=4096;
	m_memoryMapSize=tempMemoryMapSize;//4KB단위의 MemoryMapSize로 설정
	
	unsigned char flag=0xff;//모든 메모리 블록이 사용중이라면(핵심!!메모리 맵의 역활은 사용중인지 아닌지 bit단위로 표현, byte단위로 처리) 
	memset((char*)m_pMemoryMap, flag, m_memoryMapSize);//메모리 맵을 가리키는 포인터(바이트단위)에 메모리 맵 크기단위별로 사용중임을 세팅한다. 
	SetAvailableMemory((uint32_t)m_pMemoryMap, m_memorySize);//전체 메모리 사이즈에 메모리맵을 같이 전달하여 사용하지 않는 메모리만을 사용가능하게 설정한다. 
}

void* PhysicalMemoryManager::AllocBlock(){
	if(GetFreeBlockCount() <= 0)
		return NULL;
	
	int frame=GetFreeFrame();//사용하고 있지 않은 블록의 인덱스를 얻는다
	if(frame==-1)
		return NULL;
		
	SetBit(frame);//메모리 비트맵에 해당 블록이 사용중임을 세팅
	
	uint32_t addr=frame*PMM_BLOCK_SIZE;//실제 할당된 메모리 주소를 세팅(*4KB). 참고로 uint32_t는 unsigned int의 typedef형으로 4바이트로 순수양수 주소값을 나타내는데에 사용된다. 
	m_usedBlocks++;
	
	return (void*)addrl
} 

void PhycisalMemoryManager::SetBit(int bit){
	//만약 AllocBlock에서 GetFreeFrame의 리턴이 1301이라면 해당 블록이 미사용중인것이다. 메모리 비트맵은 4바이트당 32개(4byte=32bit)의 블록을 표현가능하니 
	//1301/32로 비트맵 인덱스를 얻고, 나머지인 1 즉, 두번째 비트를 세트하면 사용중임을 나타낼 수 있다. 
	// 쉽게 생각해서 메모리 비트맵은 그냥 블록별 사용여부를 bit배열로 나타내면 너무 커지니 단위를 정하여 비트연산으로 설정할 뿐이다. 
	m_pMemoryMapBit[bit/32]|=(1<<(bit%32));//메모리맵의 적절한 인덱스에 1(true)를 알맞은 자리수에 <<한다음 or로 해당 비트만 set해주면 된다. 
}
