//�⺻ ���� �ý��� �޸𸮸� 128MB�̴�. 
bool InitMemoryManager(multiboot_info* pBootInfo){//GRUB���� ��ȣ�����ý� �ʿ��� Ŀ�������� �������ִ� ����ü 
	PhysicalMemoryManager::EnablePaging(false);//GRUB���� �⺻������ ����¡�� �������� ������, Ȯ���� �ϱ����� �⺻ ����¡ ������ �� �� �ʱ�ȭ �۾��� ���� 
	PhysicalMemoryManager::Initialize(pBootInfo);
	VirtualMemeoryManager::Initialize();
	return true;
}
