//SVGA���̺귯���� ��κ��� GUI������Ʈ ����� �����ϴ�
int StartSampleGUI(){
	GuiWinThread* win_thread;
	GuiObject* obj=NULL;
	int type=SVGALIB;
	
	init_svgagui();//SVGAGUI�ʱ�ȭ 
	open_screen(type, 1024, 768, 256, "SVGAGui");//��ũ�� ��� 
	
	init_mouse();//���콺 �̹��� �ʱ�ȭ 
	kleur=BACKGROUND;
	
	win_thread=create_window_thread();//������ ������Ʈ�� ���� �� �ִ� ������ ��Ʈ ���� 
	init_interface(win_thread);//���� ������ ������Ʈ�� ������ ��Ʈ�� �߰� 
	show_window_thread(win_thread);//���ŵ� ���۸� ȭ�鿡 ��� 
	
	while(!exit_program){
		obj=do_windows(win_thread);
		if(ob==(obj->win)->kill)
			delete_window(obj->win, TRUE);
	}
	
	delete_window_thread(win_thread);
	close_screen();
	
	return 0;
} 
