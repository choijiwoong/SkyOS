//GRUB에서 보호모드로 전환 후 커널을 호출할 때, 하드웨어의 정보를 직접 얻기 어렵기에 GRUB가 커널에 넘겨주는 하드웨어 정보. 
struct multiboot_info{
	uint32_t flags;//VESA모드의 여부_비디오의 여부  
	
	uint32_t mem_lower;//BIOS로부터 얻은 이용가능한 메모리 영역 정보 
	uint32_t mem_upper;
	
	uint32_t boot_device;//부팅 디바이스의 번호
	char *cmdline;//커널에 넘기는 커맨드라인
	
	uint32_t mods_count;//부팅 모듈 리스트
	Module *Modules;
	
	union{//리눅스 파일에 대한 정보 
		AOUTSymbolTable AOUTTable;//컴파일러나 인터프리터에서 사용되는 데이터 구조 
		ELFHeaderTable ELFTable;//유닉스계열의 표준바이너리 파일 테이블 
	} SymbolTables; 
	
	uint32_t mmap_length;//메모리 매핑(파일을 프로세스의 가상주소공간 메모리에 매핑.) 정보(메모리의 특정블록 사용가능여부) 
	uint32_t mmap_addr;
	
	uint32_t drives_length;//PC에 존재하는 드라이브 정보 
	drive_info * drives_addr;
	
	ROMConfigurationTable *ConfigTable;//ROM구성테이블
	
	char* boot_loader_name;//부트로더 이름
	
	APMTable *APMTable;//APM(Apache Php Mysql) 
	
	VbeInfoBlock *vbe_control_info;
	VbeModeInfo* vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};

typedef struct multiboot_info multiboot_info_t;
