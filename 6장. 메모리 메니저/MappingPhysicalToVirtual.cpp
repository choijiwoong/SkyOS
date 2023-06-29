void MapPhysicalAddressToVirtualAddress(PageDirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags){//PDE와 PTE가 같은 플래그를 가진다고 가정. 
	kEnterCriticalSection();
	PhysicalMemoryManager::EnablePaging(false);
	
	PDE* pageDir=dir->m_entries;//PDT를 가져와서 
	if(pageDir[virt>>22]==0)//선형주소의 PDI값이 PDT에 없다면 
		CreatePageTable(dir, virt, flags);//PDE(PET)를 새로만든다 
	
	uint32_t mask=(uint32_t)(~0xfff);//앞 12비트를 제외시키는 마스크 
	uint32_t* pageTable=(uint32_t*)(pageDir[virt>>22]&mask);//PDT의 값(PTE)에 마스크를 적용하여 4KB단위로 정렬된 페이지 테이블 주소를 가져온다.(PTE처럼 PDE도 0~11이 플래그와 이용가능성, 12비트(13번쨰)부터가 주소)
	//주소값만 사용하는게 아니라 주소값의 크기를 uint32_t즉 4Byte로 맞춰야하기에 앞부분 masking한 값을 그대로 주소로 사용한다. P.169참고 
	pageTable[virt<<10>>10>>12]=phys|flags;//PET의 인덱스값(비트)에 실제 물리주소와 플래그를 설정. 즉 가상주소와 물리주소 매핑
	
	PhysicalMemoryManager::EnablePaging(true);
	kLeaveCriticalSection(); 
}

//실제 활용 예시 
using namespace PhysicalMemoryManager;
using namespace VirtualMemoryManger; 

unsigned char* physicalMemory=AllocBlocks(pProcess->m_dwPageCount);//물리 메모리 할당. 프로세스의 페이지 개수만큼 블록을 할당
for(DWORD i=0; i<pProcess->m_dwPageCount; i++){//프로세스의 페이지마다의 주소를 전부 각각 매핑. noHeaders->OptionalHeader.ImageBase는 매핑하려는 가상메모리주소, physicalMemory는 매핑하려는 물리메모리주소. 
	MapPhysicalAddressToVirtualAddress(pProcess->getPageDirectory(), ntHeaders->OptionalHeader.ImageBase + i*PAGE_SIZE, (uint32_t)physicalMemory + i*PAGE_SIZE, I86_PTE_PRESENT | I86_PTE_WRITABLE); 
} 
