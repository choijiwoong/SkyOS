//������ ���̺� ��Ʈ��
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
		I86_PTE_FRAME=0x7FFFF000//�̴� ������ ���� �ּ��̴�. 20��Ʈ�� ������ ���� 4KB�� ���ϰ� �����ּ��� �������� ���� ��Ȯ�� �����ּҸ� ��´�. 
	};
	
	void AddAttribute(PTE* entry, uint32_t attr);//�Ӽ�(�÷���)�߰� 
	void DelAttribute(PTE* entry, uint32_t attr); 
	void SetFrame(PTE* entry, uint32_t addr);//������ ����(�ּҼ���) 
	bool IsPresent(PTE entry);//�޸𸮿� �����ϴ��� 
	bool IsWritable(PTE entry); 
	uint32_t GetFrame(PTE entry);//�����ּ� ��� 
} 
