/*
SkyOS32->SkyOS64변경은 인라인 어셈블리 코드 명령어의 수정을 필요로 한다.
	1. 기존과 달리 보호모드에서 "세그멘테이션(GDT를 이용한 선형주고 변경) 과정"에서 물리메모리의 접근제약이 무시되어 64비트 영역 전체에 접근할 수 있기에 GDT(Global Descriptor Table)의 수정이 필요하다
	2. 64bit OS는 4GB이상의 메모리 공간에 접근하기위해 페이지 관련 구조체가 확장되었다.  CR#레지스터가 PML4구조체를 가리키고 PM4필드로 PML4엔트리를 얻어낸다. PML4엔트리는 페이지 디렉토리 포인터 테이블을 가리키며
	선형주소의 디렉토리 포인터 필드값을 이용해 디렉토리 포인터 엔트리를 얻어낸다.  디렉토리 포인터 엔트리를 페이지 디렉토리를 가리키고 이후는 보호모드와 같다.
		ㄴPTE(Page Table Entry)는 4바이트에서 8바이트로 확장됐다. 하위 4바이트는 보호모드와 같지만, 4GB만 접근가능하기에 상위주소와 결합해 주소에 접근한다.
	4. 호출 규약이 달라져 함수호출 시 인자를 레지스터와 스택에 쪼개어 저장한다.
	
우선 64비트 부팅을 위해 grub.cfg를 아래와 같이 수정한다
menuentry "SkyOS Loader"{
	multiboot /boot/SkyOsLoader.EXE
	module /boot/SkyOS64.sys "SKYOS64_SYS"
} 
*/

//3-1. 현재 커널에서 SKYOS64_SYS라는 커널의 이미지베이스주소와 커널 엔트리 찾기
uint32_t FindKernel64Entry(const char* szFileName, char* buf, uint32_t& imageBase){
	//1. 유요한 포맷인지
	if(!ValidatePEImage(buf)) {
		SkyConsole::Print("Invalid PE Format!! %s\n", szFileName);
		return 0;
	}
	
	IMAGE_DOS_HEADER* dosHeader=0;
	IMAGE_NT_HEADERS64* ntHeaders=0;
	dosHeader=(IMAGE_DOS_HEADER*)buf;
	
	//2. NT_HEADER 구조체 얻기_이는 파일에 개략적인 속성을 나타내는 PE형식 구조체로, FileHeader와 OptionalHeader맴버가 있다. 
	ntHeaders=(IMAGE_NT_HEADERS64*)(dosHeader->e_lfanew+(uint32_t)buf);
	SkyConsole::Print("sizeofcode0x%x\n", ntHeaders->OptionalHeader.Magic); 
	
	//3. NT_HEADER에서 커널엔트리와 이미지베이스를 얻어낸다.
	uint32_t entryPoint=(uint32_t)ntHeaders->OptionalHeader.AddressOfEntryPoint + ntHeaders->OptionalHeader.ImageBase;
	imageBase=ntHeaders->OptionalHeader.ImageBase;
	return entryPoint;
} 

void InitializePageTable(int pml4EntryAddress);//6-1. GDT를 64비트모드에 맞게 수정. 
//CR3레지스터를 0x160000으로 설정하고 PML4테이블이 위치에 존재한다. 이는 페이지디렉토리 포인터 테이블을 가리키고 페이지디렉토리 엔트리는 페이지 디렉토리를 가리킨다. 

//7-1. SkyOS/Chapter/16_SkyOS64/ModeSwitch.asm의 SwitchAndExecute64BitKernel함수: 어셈블리 오브젝트가 프로그램 앞단에 배치되는 것을 막기위해 편법으로 텍스트 섹션이 아닌 데이터섹션에 위치.
//호환모드 진입 후 64비트 모드에 진입한다.  CS세그먼트 셀렉터를 IA-32e모드(호환)용 코드 세그먼트 디스크립터로 전환 후 GDTR, GDT테이블(NULL디스크립터, 코드세그먼트디스크립터, 데이터세그먼트 디스크립터) 정의ㄴ 

void kmain(unsigned long magic, unsigned long addr){
	SkyConsole::Initialize();
	SkyConsole::Print("32Bit Kernel Entered..\n");
	
	//1. 64비트 전환이 가능한지 체크 
	if(Is64BitSwitchPossible()==false){
		SkyConsole::Print("Impossible 64bit Mode\n");
		for(;;);
	}
	
	//2. 64비트 커널이 메모리에 로드되었는지 체크 
	char* szKernelName="SKYOS64_SYS";
	multiboot_info_t* mb_info=(multiboot_info_t*)addr;
	Module* pModule=FindModule(mb_info, szKernelName);
	if(pModule==nullptr){
		SkyConsole::Print(" %s Kernel Found Fail!!\n", szKernelName);
		for(;;);
	}
	
	//3. 커널의 이미지 베이스 주소와 커널 엔트리 찾기
	uint32_t kernelEntry=0;
	uint32_t imageBase=0;
	kernelEntry=FindKernel64Entry(szKernelName, (char*)pModule->ModuleStart, imageBase);//
	if(kernelEntry==0 || imageBase==0){
		SkyConsole::Print("Invalid Kernel64 Address!!\n");
		for(;;);
	} 
	
	//4. 커널 이미지 베이스와 로드된 모듈주소 사이에 공간이 어느정도 있다고 가정하여 낭낭하게! 커널 64의 이미지베이스 로드주소는 0x200000이다 
	int pml4EntryAddress=0x160000;
	//5. 64커널 이미지베이스주소에 커널을 카피한다. 3번에서 얻은 기존 32bit커널 이미지베이스와 엔트리부분에 64bit커널로 덮어쓴다. 
	memcpy((void*)imageBase, (void*)pModule->ModuleStart, ((int)pModule->ModuleEND-(int)pModule->ModuleStart));
	//6. 54비트 모드를 위한 페이징 시스템을 구축한다. 
	InitializePageTables(pml4EntryAddress);
	//7. 페이징을 활성화하고 호환모드에 진입한다. 
	kSwitchAndExecute64bitKernel(pml4EntryAddress, KernelEntry, addr);
}
