//SVGA라이브러리로 대부분의 GUI컴포넌트 출력이 가능하다
int StartSampleGUI(){
	GuiWinThread* win_thread;
	GuiObject* obj=NULL;
	int type=SVGALIB;
	
	init_svgagui();//SVGAGUI초기화 
	open_screen(type, 1024, 768, 256, "SVGAGui");//스크린 출력 
	
	init_mouse();//마우스 이미지 초기화 
	kleur=BACKGROUND;
	
	win_thread=create_window_thread();//윈도우 컴포넌트를 담을 수 있는 윈도우 시트 생성 
	init_interface(win_thread);//샘플 윈도우 컴포넌트를 윈도우 시트에 추가 
	show_window_thread(win_thread);//갱신된 버퍼를 화면에 출력 
	
	while(!exit_program){
		obj=do_windows(win_thread);
		if(ob==(obj->win)->kill)
			delete_window(obj->win, TRUE);
	}
	
	delete_window_thread(win_thread);
	close_screen();
	
	return 0;
} 
