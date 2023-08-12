#include "rme.h"

void RequestGUIResolution(){//시스템이 지원하는 해상도 리스트를 출력한다. 
	bool result=false;
	
	//VESA관련 구조체 
	struct VesaControllerInfo* info=(VesaControllerInfo*)0x10000;
	struct VbeModeInfo *modeinfo=(VbeModeInfo*)0x9000;
	
	//V86 에뮬레이터 
	tRME_State *emu;
	
	uint16_t *zeroptr=(uint16_t*)0;
	memcpy(lowCache, zeroptr, RME_BLOCK_SIZE);//cache의 값을 RME_BLOCK_SIZE만큼 *0으로 초기화 
	
	//V86에뮬레이터 생성 
	emu=RME_CreateState();
	emu->Memory[0]=(uint8_t*)lowCache;
	for(int i=1; i<0x100000/RME_BLOCKS_ZIE ; i++)//emu->Memory공간 초기화 
		emu->Memory[i]=(uint8_t*)(i*RME_BLOCK_SIZE);
	int ret=0, mode=0;
	
	//해상도리스트 
	uint16_t* modes;
	memset(info, 0, sizeof(VesaControllerInfo));
	memcpy(info->Signature, "VBE2", 4);
	emu->AX.W=0x4F00;
	emu->ES=0x1000;
	emu->DI.W=0;
	ret=RME_CallInt(emu, 0x10);//0x10인터럽트로 결과를 보호모드에서 얻을 수 있다.(본래 16비트 리얼모드여야하는 것을 에뮬레이터로 우회) 
	
	if(info->Version<0x20 || info->Version>0x30){
		SkyConsole::Print("You have attempted to use the VESA/VBE2 driver\nwith a card that does not support VBE2.\n");
		SkyConsole::Print("System responded to VBE request with version: 0x%x\n", info->Version);
		return;
	}
	SkyCondole::Print("\nSystem responded to VBE request with version: 0x%x\n", info->Version);
	
	modes=(uint16_t*)FP_TO_LINEAR(info->Videomodes.Segment, info->Videomodes.Offset);//RME_CallInt에서 갱신된 info내의 해상도 정보를 가져와 배열로 만든다. 
	
	for(int i=1; modes[i]!=0xFFFF, ++i){//모드별로 
		emu->AX.W=0x4F01;
		emu->CX.W=modes[i];//순차적으로 해상도 정보를 세팅한 후  
		emu->ES=0x0900;
		emu->DI.W=0x0000;
		RME_CallInt(emu, 0x10);//에뮬레이터에 반영하여 
		SkyConsole::Print("%d = %dx%d:%d %d\n", i, modeinfo->XResolution, modeinfo->YResolution, modeinfo->BitsPerPixel, modeinfo->FrameBuffer);//각종 정보를 출력한다. 
	}
}

//그래픽모드 전환
bool SwitchGUIMode(int xRes, int yRes, int pixel){
	//인자사용은 안함
	.....
	emu->AX.W=0x4F01;//그래픽 모드 가능 여부 확인
	emu->CX.W=0x105;//1024*768 8비트 모드
	emu->BX.W=0x07E0;
	emu->Di.W=0;
	ret=RME_CallInt(emu, 0x10);
	
	emu->AX.W=0x4F02;//그래픽 모드 전환 요청 
	emu->BX.W=0x4105;//1024*768 8비트 모드 
	ret=RME_CallInt(emu, 0x10);
	
	return true;
}

long cmdSwitchGUI(char *theCommand){
	if(true==SwitchGUIMode(1024, 768, 261)){//그래픽 모드가 성공적으로 전환된다면 
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), 0xE0000000, 0xE0000000, 0xE0000000);//그래픽 버퍼 주소를 매핑 
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), 0xF0000000, 0xF0000000, 0xF0000000);
		
		FillRect8(100, 100, 100, 100, 8, 1024, 768);//사각형을 그린다 
		for(;;);//대기 
	}
	return false;
} 
