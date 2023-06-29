//페이지 테이블 엔트리
namespace PageTableEntry{
	typedef uint32_t PTE;
	
	enum PAGE_PTE_FLAGS{
		I86_PTE_PRESENT=1,
		I86_PTE_WRITABLE=2,
		I86_PTE_USER=4,
		I86_PTE_WRITETHOUGH=8,
		I86_PTE_NOT_CACHEABLE=0x10,
		I86_PTE_ACCESSED=0x20,
		I86_PTE_DIRTY=0x40,
		I86_PTE_PAT=0x80,
		I86_PTE_CPU_GLOBAL=0x100,
		I86_PTELV4_GLOBAL=0x200,
		I86_PTE_FRAME=0x7FFFF000//이는 페이지 기준 주소이다. 20비트로 구성된 값에 4KB를 곱하고 선형주소의 오프셋을 더해 정확한 물리주소를 얻는다. 
	};
	
	void AddAttribute(PTE* entry, uint32_t attr);//속성(플래그)추가 
	void DelAttribute(PTE* entry, uint32_t attr); 
	void SetFrame(PTE* entry, uint32_t addr);//프레임 설정(주소설정) 
	bool IsPresent(PTE entry);//메모리에 존재하는지 
	bool IsWritable(PTE entry); 
	uint32_t GetFrame(PTE entry);//물리주소 얻기 
} 
