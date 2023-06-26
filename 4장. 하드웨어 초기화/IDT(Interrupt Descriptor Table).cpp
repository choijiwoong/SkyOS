//IDT(Interrupt Descriptor Table):  하드웨어 인터럽트 혹은 예외 인터럽트(폴트, abort, 트랩)발생 시 ISR(Interrupt Service Routin)의 주소를 디스크립터셀렉터+오프셋으로 얻어오기 위한 셀렉터주소를 저장.
typedef struct tag_idt_descriptor{
	USHORT offsetLow;//오프셋 정보 
	USHORT selector;//GDT의 디스크립터를 찾아 베이스주소를 얻는다. 
	BYTE reserved;//0
	BYTE flags;
	USHORT offsetHigh;//오프셋 정보 
} idt_desciptor; 

typedef struct tag_idtr{//IDTR 레지스터에 저장할 IDT정보 
	USHORT limit;
	UINT base;
} idtr; 

bool IDTInitialize(uint16_t codeSel){
	_idtr.limit=sizeof(idt_descriptor)*I86_MAX_INTERRUPTS-1;//인터럽트 개수*인터럽트디스크립터크기로 IDTR의 크기 설정
	_idtr.base=(uint32_t)&_idt[0];//시작주소 넘기기 
	
	memset((void*)&_idt[0], 0, sizeof(idt_desctriptor)*I86_MAX_INTERRUPTS-1);//처음은 항상 NULL디스크립터 
	fot(int i=0; i<I86_MAX_INTERRUPTS; i++)
		InstallInterruptHandler(i, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, (I86_IRQ_HANDLER)InterruptDefaultHandler);//기본 핸들러를 인터럽트서비스루틴으로 설치 
		
	IDTInstall();//CPU에게 IDT의 위치를 알려준다. 
	
	return true;
}

__seclspec(naked) void InterruptDefaultHandler(){
	_asm{
		PUSHAD; //레지스터를 저장하고 
		PUSHFD
		CLI; //인터럽트 플레그 클리어 
	}
	SendEOI(); //인터럽트 종료 
	_asm{
		POPFD; //레지스터 복원 
		POPAD
		IRETD; //모든완료처리 및 원래 태스크로의 복귀 
	}
} 
