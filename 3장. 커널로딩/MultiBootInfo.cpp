//GRUB���� ��ȣ���� ��ȯ �� Ŀ���� ȣ���� ��, �ϵ������ ������ ���� ��� ��Ʊ⿡ GRUB�� Ŀ�ο� �Ѱ��ִ� �ϵ���� ����. 
struct multiboot_info{
	uint32_t flags;//VESA����� ����_������ ����  
	
	uint32_t mem_lower;//BIOS�κ��� ���� �̿밡���� �޸� ���� ���� 
	uint32_t mem_upper;
	
	uint32_t boot_device;//���� ����̽��� ��ȣ
	char *cmdline;//Ŀ�ο� �ѱ�� Ŀ�ǵ����
	
	uint32_t mods_count;//���� ��� ����Ʈ
	Module *Modules;
	
	union{//������ ���Ͽ� ���� ���� 
		AOUTSymbolTable AOUTTable;//�����Ϸ��� ���������Ϳ��� ���Ǵ� ������ ���� 
		ELFHeaderTable ELFTable;//���н��迭�� ǥ�ع��̳ʸ� ���� ���̺� 
	} SymbolTables; 
	
	uint32_t mmap_length;//�޸� ����(������ ���μ����� �����ּҰ��� �޸𸮿� ����.) ����(�޸��� Ư����� ��밡�ɿ���) 
	uint32_t mmap_addr;
	
	uint32_t drives_length;//PC�� �����ϴ� ����̺� ���� 
	drive_info * drives_addr;
	
	ROMConfigurationTable *ConfigTable;//ROM�������̺�
	
	char* boot_loader_name;//��Ʈ�δ� �̸�
	
	APMTable *APMTable;//APM(Apache Php Mysql) 
	
	VbeInfoBlock *vbe_control_info;
	VbeModeInfo* vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};

typedef struct multiboot_info multiboot_info_t;
