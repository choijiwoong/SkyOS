unsigned char normal[]={0x00, 0x1B, '1', '2', ...};//�� 

KeyBoardController::HandleKeyboardInterrupt(){
	unsigned char asciiCode;
	scanCode=InportByte(0x60);//Ű��ĵ�ڵ带 ��´�.
	if(!(SpecialKey(scanCode) | (scanCode>=0x80))){//�ƽ�Ű�ڵ��̸� 
		if(shift){//shift�� capslock�� ������ Ȯ�� 
			if(!caps)
				asciiCode=shifted[scanCode];
			else
				asciiCode=capsShifted[scanCode];
		} else{
			if(!caps)
				asciiCode=normal[scanCode];
			else
				asciiCode=capsNormal[scanCode];
		}
		
		if(buffend!=(KEYBUFFSIZE-1))
			buffend++;
		buffer[buffend]=asciiCode;
	}
}

char KeyboardController::GetInput(){
	int i=0;
	while(buffend==0)
		msleep(10);
	
	kEnterCeriticalSection();//���ͷ�Ʈ ��Ȱ��ȭ 
	for(; i<buffend; i++)//���ۼ��� 
		buffer[i]=buffer[i+1];//�ƽ�Ű���� �����ϴ� ����, �����Ͱ� ���̸� ���������� ����...?? ���� Ű���� ���� ���ϴµ�. 
	buffend--;
	kLeaveCriticalSection();//���ͷ�Ʈ Ȱ��ȭ 
	
	return buffer[0];
} 

__declspec(naked) void KeyboardHandler(){//���� �������� �������� �ʴ´�. 
	_asm{
		PUSHAD
		PUSHFD
		CLI; //���ͷ�Ʈ �ڵ鷯 ���� ���߿� �Ǵٸ� ���ͷ�Ʈ�� �߻��ϴ� ���� ���� ����� ��ɾ�. 
	}//�������� ���� �� ���ͷ�Ʈ ����
	
	//���û��� ��������� ���� �Լ��� ȣ��(asm������ �ƴ� c++ȣ���� �ǹ��ϴµ�) 
	_asm call KeyBoardController::HandleKeyboardInterrupt; //�Լ� ȣ�� 
	SendEOI();//������ �������� ���� 
	
	_asm{
		POPFD
		POPAD
		IRETD; //���ͷ�Ʈ �ڵ鷯�� �����ϰ� ������ �ڵ�� �����ϴ� ����� ��ɾ�. (���ÿ� ��Ʈ�� ����Ʈ �������� ����� ������� ���) 
	}
}

void keyBoardController::SetupInterrupts(){
	FlushBuffers();
	setvect(33, KeyboardHandler);
}

bool InitKeyboard(){
	KeyBoardController::SetupInterrupts();
	return true;
} 


