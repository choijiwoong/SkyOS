void MapPhysicalAddressToVirtualAddress(PageDirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags){//PDE�� PTE�� ���� �÷��׸� �����ٰ� ����. 
	kEnterCriticalSection();
	PhysicalMemoryManager::EnablePaging(false);
	
	PDE* pageDir=dir->m_entries;//PDT�� �����ͼ� 
	if(pageDir[virt>>22]==0)//�����ּ��� PDI���� PDT�� ���ٸ� 
		CreatePageTable(dir, virt, flags);//PDE(PET)�� ���θ���� 
	
	uint32_t mask=(uint32_t)(~0xfff);//�� 12��Ʈ�� ���ܽ�Ű�� ����ũ 
	uint32_t* pageTable=(uint32_t*)(pageDir[virt>>22]&mask);//PDT�� ��(PTE)�� ����ũ�� �����Ͽ� 4KB������ ���ĵ� ������ ���̺� �ּҸ� �����´�.(PTEó�� PDE�� 0~11�� �÷��׿� �̿밡�ɼ�, 12��Ʈ(13����)���Ͱ� �ּ�)
	//�ּҰ��� ����ϴ°� �ƴ϶� �ּҰ��� ũ�⸦ uint32_t�� 4Byte�� ������ϱ⿡ �պκ� masking�� ���� �״�� �ּҷ� ����Ѵ�. P.169���� 
	pageTable[virt<<10>>10>>12]=phys|flags;//PET�� �ε�����(��Ʈ)�� ���� �����ּҿ� �÷��׸� ����. �� �����ּҿ� �����ּ� ����
	
	PhysicalMemoryManager::EnablePaging(true);
	kLeaveCriticalSection(); 
}

//���� Ȱ�� ���� 
using namespace PhysicalMemoryManager;
using namespace VirtualMemoryManger; 

unsigned char* physicalMemory=AllocBlocks(pProcess->m_dwPageCount);//���� �޸� �Ҵ�. ���μ����� ������ ������ŭ ����� �Ҵ�
for(DWORD i=0; i<pProcess->m_dwPageCount; i++){//���μ����� ������������ �ּҸ� ���� ���� ����. noHeaders->OptionalHeader.ImageBase�� �����Ϸ��� ����޸��ּ�, physicalMemory�� �����Ϸ��� �����޸��ּ�. 
	MapPhysicalAddressToVirtualAddress(pProcess->getPageDirectory(), ntHeaders->OptionalHeader.ImageBase + i*PAGE_SIZE, (uint32_t)physicalMemory + i*PAGE_SIZE, I86_PTE_PRESENT | I86_PTE_WRITABLE); 
} 
