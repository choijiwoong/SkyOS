//GDT(Global Descriptor Table): 보호보드에서 물리 주소에 접근이 가능한지 주소범위의 유효성, DPL레벨기반 접근가능여부를 확인해주는 구조체
typedef struct tag_gdtDescriptor{
	USHORT segmentLimit;//16비트 크기 
	USHORT baseLow;//베이스 주소 
	BYTE baseMiddle;//주소 
	BYTE flags;//grand역시 주소를 가리키는가 
	BYTE grand;//4비트 추가크기(도합 20비트인데, 이를 <<12하여 32비트 세그먼트 크기 즉 4GB를 표현한다) 
	BYTE baseHigh;//주소 
}gdtDescriptor; 

//GDTR(Global Descriptor Table Register): GPU는 GDTR을 참조하여 GDT에 접근하이게 어셈블리 명령 lgdt오 GDTR레지스터에 값을 설정한다.
typedef struct tag_gdtr{//6바이트 GDTR구조체. lgdt gdtr;로 등록한다. 
	USHORT m_limit;//GDT의 크기 
	UINT m_base;//GDT의 시작주소 
};gdtr 

//GDT설정
int GDTInitialize(){//모든 디스크립터 세그먼트 베이스주소를 0, 세그먼트 크기를 4GB로 설정 
	//GDTR에 로드될 _gdtr을 초기화한다.
	//_gdtr의 주소는 실제 물리주소이며, MAX_DESCRIPTOR는 5개(NULL 디스크립터, 커널코드 디스크립터, 커널데이터 디스크립터, 유저코드 디스크립터, 유저데이터 디스크립터)이다. 크기는 각 8바이트로 GDT는 40바이트이다.
	_gdtr.m_limit=(sizeof(struct gdt_descriptor)*MAX_DESCRIPTOR)-1;
	_gdtr.m_base=(uint32_t)&_gdt[0];//시작주소를 GDT의 시작주소로.
	
	//1. NULL 디스크립터 설정: 디스크립터 테이블 내의 첫번째 디스크립터는 항상 NULL로 설정 
	get_set_descriptor(0,0,0,0,0);
	
	//2. 커널코드 디스크립터 설정: 커널코드실행 시 접근권한을 기술 
	gdt_set_descriptor(1,0,0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY, //읽고쓰기가능, 코드세그먼트(default는 데이터),  시스템정의세그먼트면0 코드나데이터세그먼트면 1, 이 세그먼트는 물리메로리에 올라와있어 접근가능하다 
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);//세그먼트단위 4K, 32비트모드, grand비트설정시 이 4바이트를 포함하여 20비트로 4GB공간을 표현. 
	
	//3. 커넣데이터 디스크립터 설정: 커넣데이터영역에 WR할때 접근권한을 기술 
	gdt_set_descriptor(2,0,0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY,//읽고쓰기가능, 시스템정의시 0 코드나데이터면 1, 물리메모리에 올라와있기에 접근가능 
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);//세그먼트단위 4K, 32비트모드, 추가주소데이터 20bit로 4GB 
	
	//4. 유저모드 코드 디스크립터 설정: 유저코드실행 시 접근권한을 기술 
	gdt_set_descriptor(3, 0, 0xffffffff
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY | I86_GDT_DESC_DPL,//읽고쓰기가능, 코드세그먼트, 시스템정의시0 코드나데이터면1, 물리메모리이기에 접근가능, DPL특권 레벨 
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);//세그먼트단위 4K, 32비트모드, 추가주소 
	
	//5. 유저모드 데이터 디스크립터 설정: 유저데이터영역에 접근 시 접근권한을 기술 
	gdt_set_descriptor(4,0,0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_DPL, //읽쓰, 시스템정의시 0 코드나데이터면 1, DPL특권레벨 
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);//세그먼트 단위 4K, 32비트모드, 추가주소 
	
	//GDTR레지스터에 GDT 로드. 그 외 플래그로 I86_GDT_DESC_ACCESS(어떤 스레드가 접근시 1_엑세스비트), I86_GDT_DESC_EXPANSION(확장비트), I86_GDT_GRAND_OS(64비트 네이티브 코드를 포함하고 있는가) 
	gdt_install();
	
	return 0; 
} 
