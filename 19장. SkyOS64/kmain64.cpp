//64비트에서는 long, 포인터, long double, size_t ssize_t등의 자료형 크기차이도 주의해야한다. 32비트로 컴파일된 GRUB정보구조체같은 경우가 문제이다. 크기를 맞추어 포인터는 intptr_t, long은 int로 변경한다. 
void kmain64(int addr){
	InitializeConstructors();//글로벌 혹은 정적객체를 선언하지 못했던 GRUB의 제약이 64비트커널의 엔트리 주소는 가상주소 어디에서도 문제가 없기에 제약이 사라졌다. 
	
	multiboot_info* pBootInfo=(multiboot_info*)addr;
	
	SkyConsole::Initialize();
	SkyConsole::Print("SkyOS64 Kernel Entered..\n");
	
	SkyConsole::Print("GRUB Information\n");
	SkyConsole::Print("Boot Loader Name: %s\n", pBootInfo->boot_loader_name);//제대로 GRUB정보가 전달되었는지. 
	
	SkyConsole::Print("Hello World64!\n");
	for(;;);
}
