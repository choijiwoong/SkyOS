//PIT(Programmable Interrupt Timer): 일반적인 타이머 제어를 위한 기능으로 타이머 인터럽트 형태로 CPU에 32번 인터럽트 신호를 제공한다. 
void InitializePIT(){//PIT초기화(등록) 
	setvect(32, InterrputPITHandler);//PIT핸들러를 CPU의 32번 인터럽트 벡터로 세팅한다. 
}

void StartPITCounter(uint32_t freq, uint8_t counter, uint8_t mode){//진동수, 사용할 카운터 레지스터(0,1,2,3이 존재), 카운팅 방식 
	if(freq==0)//진동수가 0이면 종료 1s/freq마다 실행되니.. 0division 
		return;
	
	uint16_t divisor=uint16_t(1193181 / (uint16_t)freq);//PIT는 초당틱 1193181이기에 시간단위(틱) 계산ㄴ 
	
	uint8_t ocw=0;//사용할 명령어 초기화 
	ocw=(ocw & ~I86_PIT_OCW_MASK_MODE) | mode;//카운팅 방식으로 PIT 초기화 
	ocw=(ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;//모르겠다.. 
	ocw=(ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;//사용할 카운터 레지스터 세팅 
	SendPICCommand(ocw);//PIC에게 PIT 커맨드 전송
	
	SendPITData(divisor & 0xff, 0);//마스터의 32번 안터럽트 핸들러를 호출..?
	SendPITData((divisor >> 8) & 0xff, 0);//프리퀀시 비율 설정 
	
	_pitTicks=0; //타이머(틱 카운트) 리셋 
}

//아래와 같이 사용
StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);//카운터 0~2는 읽고쓰기가 가능하고, 3은 쓰기만 가능하다. 
