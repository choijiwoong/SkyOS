void kmain(unsigned long magic, unsigned long addr){
	......
	//����&����޸𸮸Ŵ��� �ʱ�ȭ, ���� �ý��� �޸� 128MB����
	InitMemoryManager(pBootInfo);
	SkyConsole::Print("Memory Manger Init Complete\n");
	
	int heapFrameCount=256*10*5;//�����Ӽ��� 12800��, 52MB
	unsigned int requiredHeapSize=heapFrameCount*PAGE_SIZE;
	
	if(requiredHeapSize > g_freeMemorySize){//��������ũ��� ���Ͽ� Ŭ �� 
		requiredHeapSize=g_freeMemorySize;//������ �ִ� ũ��� ���߰�
		heapFrameCount=requiredHeapSize/PAGE_SIZE/2;//�䱸�� ũ�⸦ ������ ���δ� 
	} 
	HeapManager::InitKernelHeap(heapFrameCount);
	SkyConsole::Print("Heap %dMB Allocated\n", requiredHeapSize/1048576);
	.....
} 
//������ 0x111000���� �޸� ��� ��Ʈ�� �迭��, 0x13C000���� ���� ��ġ�Ѵ�. 
//�����ּҴ� 0x10000000�̸� 256MB�� �����Ѵ�. 
