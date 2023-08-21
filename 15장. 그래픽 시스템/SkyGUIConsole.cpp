/*
콘솔모드와 그래픽모드의 출력버퍼가 다르기에 새로운 그래픽 버퍼 주소에 문자열을 찍을 수 있어야 한다. 
SkyGUI는 8비트 팔레트를 사용하여 256색을 표현할 수 있다. 
*/
#define TIMEOUT_PER_SECOND 1000

class SkyGUIConsole{
	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run(); 
	bool Print(char* pMsg);//출력버퍼가 콘솔과 다르기에 새로히 정의 
	void GetNewLine();
	bool Clear();
	void FillRect(int x, int y, int w, int h, int col);
	.....
	
	protected:
		void PutCursor();
		
	private:
		.....
		SkyRender* m_pRenderer;
		int m_yPos;
		int m_xPos;
};

DWORD WINAPI GUIWatchDogProc(LPVOID parameter){
	Process* pProcess=(Process*)parameter;
	int pos=0;
	int colorStatus[]={0x00FF0000, 0x0000FF00, 0x0000FF};
	int first=GetTickCount();
	
	ULONG* lfb=(ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;//그래픽버퍼주소
	while(1){//우측 상단에 사각형을 그린다 
		int second=GetTickCount();
		if(second-first>=TIMEOUT_PER_SECOND){
			if(++pos>2)//색깔 변경을 위함(1초당) 
				pos=0;
			
			if(m_bShowTSWatchdogClock)
				SampleFillRect(lfb, 1004, 0, 20, 20, colorStatus[pos]);
			
			first=GetTickCount();
		}
		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());//빠르게 실행되야 하기에 타 프로세스에 양보한다 
	} 
	return 0;
}

bool SkyGUIConsole::Run(){
	ProcessManager::GetInstance()->CreateProcessFromMemory("GUIWatchDog", GUIWatchDogProc, NULL, PROCESS_KERNEL);
}

//WIN32스타일 코딩
int APIWNTRY wWinMain{
	.....
	MyRegisterClass(hInstance);
	if(!InitInstance(hInstance, nCmdShow))
		return FALSE;
	
	MSG msg;
	
	while(GetMessage(&msg, nullptr, 0, 0)){
		if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
} 

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){//초기 WIN스타일을 그대로 차용하나, WM_COMMAND 시 기본윈도우프로시저(정의한)를 리턴하게끔 한다. 
	switch(message){
		case WM_COMMAND:
			{
				int wmId=LOWORD(wParam);
				switch(wmId){
					.....
					default:
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;
		
		.....
		
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
