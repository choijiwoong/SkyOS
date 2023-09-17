/*
DLL������ �ھ������� �׻� 0x100000�� �ε�Ǵ� �Ͱ� �޸� ��ȣ�Ǵ� ������ �����Ǿ����� �ʴ�.
������ �⺻ ������ EntryPoint�̱⿡ DllMain���� ����� �������� ��������� �Ѵ�.
 DLL�ε��� GRUB�޴���  module /boot/SampleDll.dll "SAMPLE_DLL" �߰�
�Ϲ����� DLL�� �޸� ���� �ε���� ���ٸ� �ּҸ����� �ʿ����� ������ ���� �ڵ����簡�ƴ� ������������ ���� �����̱⿡
WIN32���� ���� �ڵ����Ӽ��� ����. ���μ��� ������ �̿��� ���·�, �̷��� ������ ���� ���� ���μ����� DLL�ڵ尡 ��������
������ �ε�ƴ����� Ȯ���ؾ��Ѵ�. ������ ������ ������ DLL�� ������ �����ּҿ� �����ؾ��Ѵ�. 
*/
#include "ServiceFunc.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){ 
	//���� WIN32 DLL�� ���μ��� ���� ȣ��(kEnterCriticalSection, kLeaveCriticalSection). SkyOS������ ����ī��Ʈ�� ����(��Ű���� Ȯ��� ����ȭ ����ʼ�) 
	switch(ul_reason_for_call){
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

bool TestMemoryModule(const char* moduleName){
	//��� �ε�, �ڵ��ȯ 
	MODULE_HANDLE hwnd=SkyModuleManager::GetInstance()->LoadModuleFromMemory(moduleName);
	//����� �������ִ� GetDLLInterface��� �޼����� �ּҸ� ��´�. (�������̽��� ����) 
	PGetDLLInterface GetDLLInterface=(PGetDLLInterface)SkyModuleManager::GetInstance()->GetModuleFunction((MODULE_HANDLE)(hwnd), "GetDLLInterface");
	//�ش� �Լ��� ȣ���Ͽ� DLL�������̽��� ��´�? 
	const DLLInterface* dll_interface=GetDLLInterface();
	//�������̽� ���ΰ� �ñ��ϱ��ѵ�, �������̽��� ���� �Լ� ȣ�� �� ��� ��� 
	int sum=dll_interface->AddNumber(5,6);
	SkyConsole::Print("AddNumber(5,6): %d\n", sum);
	//��� ����. ��������Ʈ�� �����ٵ� 
	SkyModuleManager::GetInstance()->UnloadModule((MODULE_HANDLE)(hwnd));
}
