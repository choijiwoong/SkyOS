//기본 설정 시스템 메모리를 128MB이다. 
bool InitMemoryManager(multiboot_info* pBootInfo){//GRUB에서 보호모드부팅시 필요한 커널정보를 전달해주는 구조체 
	PhysicalMemoryManager::EnablePaging(false);//GRUB에서 기본적으로 페이징을 수행하진 않지만, 확실히 하기위해 기본 페이징 설정을 끈 뒤 초기화 작업을 수행 
	PhysicalMemoryManager::Initialize(pBootInfo);
	VirtualMemeoryManager::Initialize();
	return true;
}
