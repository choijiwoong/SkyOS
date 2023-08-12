#include "rme.h"

void RequestGUIResolution(){//�ý����� �����ϴ� �ػ� ����Ʈ�� ����Ѵ�. 
	bool result=false;
	
	//VESA���� ����ü 
	struct VesaControllerInfo* info=(VesaControllerInfo*)0x10000;
	struct VbeModeInfo *modeinfo=(VbeModeInfo*)0x9000;
	
	//V86 ���ķ����� 
	tRME_State *emu;
	
	uint16_t *zeroptr=(uint16_t*)0;
	memcpy(lowCache, zeroptr, RME_BLOCK_SIZE);//cache�� ���� RME_BLOCK_SIZE��ŭ *0���� �ʱ�ȭ 
	
	//V86���ķ����� ���� 
	emu=RME_CreateState();
	emu->Memory[0]=(uint8_t*)lowCache;
	for(int i=1; i<0x100000/RME_BLOCKS_ZIE ; i++)//emu->Memory���� �ʱ�ȭ 
		emu->Memory[i]=(uint8_t*)(i*RME_BLOCK_SIZE);
	int ret=0, mode=0;
	
	//�ػ󵵸���Ʈ 
	uint16_t* modes;
	memset(info, 0, sizeof(VesaControllerInfo));
	memcpy(info->Signature, "VBE2", 4);
	emu->AX.W=0x4F00;
	emu->ES=0x1000;
	emu->DI.W=0;
	ret=RME_CallInt(emu, 0x10);//0x10���ͷ�Ʈ�� ����� ��ȣ��忡�� ���� �� �ִ�.(���� 16��Ʈ �����忩���ϴ� ���� ���ķ����ͷ� ��ȸ) 
	
	if(info->Version<0x20 || info->Version>0x30){
		SkyConsole::Print("You have attempted to use the VESA/VBE2 driver\nwith a card that does not support VBE2.\n");
		SkyConsole::Print("System responded to VBE request with version: 0x%x\n", info->Version);
		return;
	}
	SkyCondole::Print("\nSystem responded to VBE request with version: 0x%x\n", info->Version);
	
	modes=(uint16_t*)FP_TO_LINEAR(info->Videomodes.Segment, info->Videomodes.Offset);//RME_CallInt���� ���ŵ� info���� �ػ� ������ ������ �迭�� �����. 
	
	for(int i=1; modes[i]!=0xFFFF, ++i){//��庰�� 
		emu->AX.W=0x4F01;
		emu->CX.W=modes[i];//���������� �ػ� ������ ������ ��  
		emu->ES=0x0900;
		emu->DI.W=0x0000;
		RME_CallInt(emu, 0x10);//���ķ����Ϳ� �ݿ��Ͽ� 
		SkyConsole::Print("%d = %dx%d:%d %d\n", i, modeinfo->XResolution, modeinfo->YResolution, modeinfo->BitsPerPixel, modeinfo->FrameBuffer);//���� ������ ����Ѵ�. 
	}
}

//�׷��ȸ�� ��ȯ
bool SwitchGUIMode(int xRes, int yRes, int pixel){
	//���ڻ���� ����
	.....
	emu->AX.W=0x4F01;//�׷��� ��� ���� ���� Ȯ��
	emu->CX.W=0x105;//1024*768 8��Ʈ ���
	emu->BX.W=0x07E0;
	emu->Di.W=0;
	ret=RME_CallInt(emu, 0x10);
	
	emu->AX.W=0x4F02;//�׷��� ��� ��ȯ ��û 
	emu->BX.W=0x4105;//1024*768 8��Ʈ ��� 
	ret=RME_CallInt(emu, 0x10);
	
	return true;
}

long cmdSwitchGUI(char *theCommand){
	if(true==SwitchGUIMode(1024, 768, 261)){//�׷��� ��尡 ���������� ��ȯ�ȴٸ� 
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), 0xE0000000, 0xE0000000, 0xE0000000);//�׷��� ���� �ּҸ� ���� 
		VirtualMemoryManager::CreateVideoDMAVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), 0xF0000000, 0xF0000000, 0xF0000000);
		
		FillRect8(100, 100, 100, 100, 8, 1024, 768);//�簢���� �׸��� 
		for(;;);//��� 
	}
	return false;
} 
