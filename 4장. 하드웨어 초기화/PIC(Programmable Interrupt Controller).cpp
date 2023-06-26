//PIC(Programmable Interrupt Controller): 이벤트 인터럽트 시 PIC는 CPU에게 신호가담긴 INT를 보내고, CPU는 EFLAG의 IE비트를 세팅 후 PIC에게 오류발생전달완료신호를 보낸다. 그뒤 PIC는 INTA를 통해 인터럽트 출처를 
//데이터버스를 통해 CPU에게 전달하고, CPU가 보호모드라면 IDT 디스크립터를 찾아 인터럽트 핸들러를 실행한다. 슬레이브에서 인터럽트 시 마스터 PIC IRQ 2번에 인터럽트 신호를 보낸다.
 //각 장치별 인터럽트를 의미하는 IRQ핀을 먼저 초기화해줘야한다. 이때 ICW를 사용한다. 
void PICInitialize(uint8_t base0, uint8_t base1){
	uint8_t icw=0;//ICW(Initialization Control Word)로 마스터 PIC의 명령레지스터로 명령을 전달하는데에 사용한다.
	
	icw=(icw & ~I86_PIC_ICW1_MASK_INIT) | I86_PIC_ICW1_INIT_YES;//PIC 초기화
	icw=(icw & ~I86_PIC_ICW1_MASK_IC4) | I86_ICW1_IC4_EXPECT;
	
	SendCommandToPIC(icw, 0);//ICW1명령어(PIC초기화)를 마스터0과 슬레이브1에 전송.  
	SendCommandToPIC(icw, 1); 
	
	SendDataToPIC(base0, 0);//IRQ의 베이스 주소인 base0과 base1에 각각 ICW2명령(IRQ오프셋을 더할 베이스주소)을 보낸다. 
	SendDataToPIC(base1, 1);//마스터와 슬레이브에게 각각의 베이스주소 전달 
	
	SendDataToPIC(0x04, 0);//PIC에 ICW3명령(마스터와 슬레이브 연결방식 지정)을 보낸다. 마스터와 슬레이브 PIC와의 관계 정립 
	SendDataToPIC(0x02, 1);//마스터와 슬레이브에게 연결방식 지정 
	
	icw=(icw & ~I86_PIC_ICW4_MASK_UPM) | I86_PIC_ICW4_UPM_86MODE;//ICW4명령(추가명령)을 보낸다. i86모드 활성화
	
	SendDataToPIC(icw, 0);//PIC 초기화 완료 
	SendDataToPIC(icw, 1); //추가명령어를 마스터와 슬레이브에게 전송. 
}// ReadDataFromPIC: PIC에서 1byte읽기, SendDataToPIC: PIC로 데이터 전송, SendCommandToPIC: PIC로 명령어 전송. 내부적으로 OutPortByte, OutPortWord, OutPortDWord, InPortByte, InPortWord, InPortDWord를 사용하여 1, 2, 4바이트단위로 읽고쓴다. 

//ICW명령은 4가지의 초기화 명령어로 구성되는데 1부터 4까지 차례로 PIC초기화,  PIC가 인터럽트를 받았을 떄 IRQ에 얼마를 더해 CPU에게 보낼지 설정,
//PIC의 마스터와 슬레이브 연결방법 지정, 추가명령어로 이루어진다. 
