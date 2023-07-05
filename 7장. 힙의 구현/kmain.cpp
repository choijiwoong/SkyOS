void kmain(unsigned long magic, unsigned long addr){
	......
	//물리&가상메모리매니저 초기화, 설정 시스템 메모리 128MB가정
	InitMemoryManager(pBootInfo);
	SkyConsole::Print("Memory Manger Init Complete\n");
	
	int heapFrameCount=256*10*5;//프레임수는 12800개, 52MB
	unsigned int requiredHeapSize=heapFrameCount*PAGE_SIZE;
	
	if(requiredHeapSize > g_freeMemorySize){//자유공간크기와 비교하여 클 때 
		requiredHeapSize=g_freeMemorySize;//가능한 최대 크기로 맞추고
		heapFrameCount=requiredHeapSize/PAGE_SIZE/2;//요구한 크기를 반으로 줄인다 
	} 
	HeapManager::InitKernelHeap(heapFrameCount);
	SkyConsole::Print("Heap %dMB Allocated\n", requiredHeapSize/1048576);
	.....
} 
//실행결과 0x111000에는 메모리 블록 비트맵 배열이, 0x13C000부터 힙이 위치한다. 
//가상주소는 0x10000000이며 256MB로 시작한다. 
