/*
SkyOS32->SkyOS64������ �ζ��� ����� �ڵ� ��ɾ��� ������ �ʿ�� �Ѵ�.
	1. ������ �޸� ��ȣ��忡�� "���׸����̼�(GDT�� �̿��� �����ְ� ����) ����"���� �����޸��� ���������� ���õǾ� 64��Ʈ ���� ��ü�� ������ �� �ֱ⿡ GDT(Global Descriptor Table)�� ������ �ʿ��ϴ�
	2. 64bit OS�� 4GB�̻��� �޸� ������ �����ϱ����� ������ ���� ����ü�� Ȯ��Ǿ���.  CR#�������Ͱ� PML4����ü�� ����Ű�� PM4�ʵ�� PML4��Ʈ���� ����. PML4��Ʈ���� ������ ���丮 ������ ���̺��� ����Ű��
	�����ּ��� ���丮 ������ �ʵ尪�� �̿��� ���丮 ������ ��Ʈ���� ����.  ���丮 ������ ��Ʈ���� ������ ���丮�� ����Ű�� ���Ĵ� ��ȣ���� ����.
		��PTE(Page Table Entry)�� 4����Ʈ���� 8����Ʈ�� Ȯ��ƴ�. ���� 4����Ʈ�� ��ȣ���� ������, 4GB�� ���ٰ����ϱ⿡ �����ּҿ� ������ �ּҿ� �����Ѵ�.
	4. ȣ�� �Ծ��� �޶��� �Լ�ȣ�� �� ���ڸ� �������Ϳ� ���ÿ� �ɰ��� �����Ѵ�.
	
�켱 64��Ʈ ������ ���� grub.cfg�� �Ʒ��� ���� �����Ѵ�
menuentry "SkyOS Loader"{
	multiboot /boot/SkyOsLoader.EXE
	module /boot/SkyOS64.sys "SKYOS64_SYS"
} 
*/

//3-1. ���� Ŀ�ο��� SKYOS64_SYS��� Ŀ���� �̹������̽��ּҿ� Ŀ�� ��Ʈ�� ã��
uint32_t FindKernel64Entry(const char* szFileName, char* buf, uint32_t& imageBase){
	//1. ������ ��������
	if(!ValidatePEImage(buf)) {
		SkyConsole::Print("Invalid PE Format!! %s\n", szFileName);
		return 0;
	}
	
	IMAGE_DOS_HEADER* dosHeader=0;
	IMAGE_NT_HEADERS64* ntHeaders=0;
	dosHeader=(IMAGE_DOS_HEADER*)buf;
	
	//2. NT_HEADER ����ü ���_�̴� ���Ͽ� �������� �Ӽ��� ��Ÿ���� PE���� ����ü��, FileHeader�� OptionalHeader�ɹ��� �ִ�. 
	ntHeaders=(IMAGE_NT_HEADERS64*)(dosHeader->e_lfanew+(uint32_t)buf);
	SkyConsole::Print("sizeofcode0x%x\n", ntHeaders->OptionalHeader.Magic); 
	
	//3. NT_HEADER���� Ŀ�ο�Ʈ���� �̹������̽��� ����.
	uint32_t entryPoint=(uint32_t)ntHeaders->OptionalHeader.AddressOfEntryPoint + ntHeaders->OptionalHeader.ImageBase;
	imageBase=ntHeaders->OptionalHeader.ImageBase;
	return entryPoint;
} 

void InitializePageTable(int pml4EntryAddress);//6-1. GDT�� 64��Ʈ��忡 �°� ����. 
//CR3�������͸� 0x160000���� �����ϰ� PML4���̺��� ��ġ�� �����Ѵ�. �̴� ���������丮 ������ ���̺��� ����Ű�� ���������丮 ��Ʈ���� ������ ���丮�� ����Ų��. 

//7-1. SkyOS/Chapter/16_SkyOS64/ModeSwitch.asm�� SwitchAndExecute64BitKernel�Լ�: ����� ������Ʈ�� ���α׷� �մܿ� ��ġ�Ǵ� ���� �������� ������� �ؽ�Ʈ ������ �ƴ� �����ͼ��ǿ� ��ġ.
//ȣȯ��� ���� �� 64��Ʈ ��忡 �����Ѵ�.  CS���׸�Ʈ �����͸� IA-32e���(ȣȯ)�� �ڵ� ���׸�Ʈ ��ũ���ͷ� ��ȯ �� GDTR, GDT���̺�(NULL��ũ����, �ڵ弼�׸�Ʈ��ũ����, �����ͼ��׸�Ʈ ��ũ����) ���Ǥ� 

void kmain(unsigned long magic, unsigned long addr){
	SkyConsole::Initialize();
	SkyConsole::Print("32Bit Kernel Entered..\n");
	
	//1. 64��Ʈ ��ȯ�� �������� üũ 
	if(Is64BitSwitchPossible()==false){
		SkyConsole::Print("Impossible 64bit Mode\n");
		for(;;);
	}
	
	//2. 64��Ʈ Ŀ���� �޸𸮿� �ε�Ǿ����� üũ 
	char* szKernelName="SKYOS64_SYS";
	multiboot_info_t* mb_info=(multiboot_info_t*)addr;
	Module* pModule=FindModule(mb_info, szKernelName);
	if(pModule==nullptr){
		SkyConsole::Print(" %s Kernel Found Fail!!\n", szKernelName);
		for(;;);
	}
	
	//3. Ŀ���� �̹��� ���̽� �ּҿ� Ŀ�� ��Ʈ�� ã��
	uint32_t kernelEntry=0;
	uint32_t imageBase=0;
	kernelEntry=FindKernel64Entry(szKernelName, (char*)pModule->ModuleStart, imageBase);//
	if(kernelEntry==0 || imageBase==0){
		SkyConsole::Print("Invalid Kernel64 Address!!\n");
		for(;;);
	} 
	
	//4. Ŀ�� �̹��� ���̽��� �ε�� ����ּ� ���̿� ������ ������� �ִٰ� �����Ͽ� �����ϰ�! Ŀ�� 64�� �̹������̽� �ε��ּҴ� 0x200000�̴� 
	int pml4EntryAddress=0x160000;
	//5. 64Ŀ�� �̹������̽��ּҿ� Ŀ���� ī���Ѵ�. 3������ ���� ���� 32bitĿ�� �̹������̽��� ��Ʈ���κп� 64bitĿ�η� �����. 
	memcpy((void*)imageBase, (void*)pModule->ModuleStart, ((int)pModule->ModuleEND-(int)pModule->ModuleStart));
	//6. 54��Ʈ ��带 ���� ����¡ �ý����� �����Ѵ�. 
	InitializePageTables(pml4EntryAddress);
	//7. ����¡�� Ȱ��ȭ�ϰ� ȣȯ��忡 �����Ѵ�. 
	kSwitchAndExecute64bitKernel(pml4EntryAddress, KernelEntry, addr);
}
