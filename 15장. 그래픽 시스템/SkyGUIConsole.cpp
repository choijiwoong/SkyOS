/*
�ָܼ��� �׷��ȸ���� ��¹��۰� �ٸ��⿡ ���ο� �׷��� ���� �ּҿ� ���ڿ��� ���� �� �־�� �Ѵ�. 
SkyGUI�� 8��Ʈ �ȷ�Ʈ�� ����Ͽ� 256���� ǥ���� �� �ִ�. 
*/
#define TIMEOUT_PER_SECOND 1000

class SkyGUIConsole{
	bool Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype);
	bool Run(); 
	bool Print(char* pMsg);//��¹��۰� �ְܼ� �ٸ��⿡ ������ ���� 
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
	
	ULONG* lfb=(ULONG*)SkyGUISystem::GetInstance()->GetVideoRamInfo()._pVideoRamPtr;//�׷��ȹ����ּ�
	while(1){//���� ��ܿ� �簢���� �׸��� 
		int second=GetTickCount();
		if(second-first>=TIMEOUT_PER_SECOND){
			if(++pos>2)//���� ������ ����(1�ʴ�) 
				pos=0;
			
			if(m_bShowTSWatchdogClock)
				SampleFillRect(lfb, 1004, 0, 20, 20, colorStatus[pos]);
			
			first=GetTickCount();
		}
		Scheduler::GetInstance()->Yield(pProcess->GetProcessId());//������ ����Ǿ� �ϱ⿡ Ÿ ���μ����� �纸�Ѵ� 
	} 
	return 0;
}

bool SkyGUIConsole::Run(){
	ProcessManager::GetInstance()->CreateProcessFromMemory("GUIWatchDog", GUIWatchDogProc, NULL, PROCESS_KERNEL);
}
