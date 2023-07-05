unsigned char normal[]={0x00, 0x1B, '1', '2', ...};//등 

KeyBoardController::HandleKeyboardInterrupt(){
	unsigned char asciiCode;
	scanCode=InportByte(0x60);//키스캔코드를 얻는다.
	if(!(SpecialKey(scanCode) | (scanCode>=0x80))){//아스키코드이면 
		if(shift){//shift와 capslock의 유무를 확인 
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
	
	kEnterCeriticalSection();//인터럽트 비활성화 
	for(; i<buffend; i++)//버퍼수정 
		buffer[i]=buffer[i+1];//아스키값을 저장하는 버퍼, 데이터가 쌓이면 순차적으로 얻어낸다...?? 대충 키보드 버퍼 말하는듯. 
	buffend--;
	kLeaveCriticalSection();//인터럽트 활성화 
	
	return buffer[0];
} 

__declspec(naked) void KeyboardHandler(){//스택 프레임을 형성하지 않는다. 
	_asm{
		PUSHAD
		PUSHFD
		CLI; //인터럽트 핸들러 실행 도중에 또다른 인터럽트가 발생하는 것을 막는 어셈블리 명령어. 
	}//레지스터 저장 및 인터럽트 끄기
	
	//스택상태 변경방지를 위해 함수를 호출(asm에서가 아닌 c++호출을 의미하는듯) 
	_asm call KeyBoardController::HandleKeyboardInterrupt; //함수 호출 
	SendEOI();//수행결과 오류여부 전송 
	
	_asm{
		POPFD
		POPAD
		IRETD; //인터럽트 핸들러를 종료하고 원래의 코드로 복원하는 어셈블리 명령어. (스택에 엔트리 포인트 정보들이 제대로 들어있을 경우) 
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


