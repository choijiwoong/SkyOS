//64��Ʈ������ long, ������, long double, size_t ssize_t���� �ڷ��� ũ�����̵� �����ؾ��Ѵ�. 32��Ʈ�� �����ϵ� GRUB��������ü���� ��찡 �����̴�. ũ�⸦ ���߾� �����ʹ� intptr_t, long�� int�� �����Ѵ�. 
void kmain64(int addr){
	InitializeConstructors();//�۷ι� Ȥ�� ������ü�� �������� ���ߴ� GRUB�� ������ 64��ƮĿ���� ��Ʈ�� �ּҴ� �����ּ� ��𿡼��� ������ ���⿡ ������ �������. 
	
	multiboot_info* pBootInfo=(multiboot_info*)addr;
	
	SkyConsole::Initialize();
	SkyConsole::Print("SkyOS64 Kernel Entered..\n");
	
	SkyConsole::Print("GRUB Information\n");
	SkyConsole::Print("Boot Loader Name: %s\n", pBootInfo->boot_loader_name);//����� GRUB������ ���޵Ǿ�����. 
	
	SkyConsole::Print("Hello World64!\n");
	for(;;);
}
