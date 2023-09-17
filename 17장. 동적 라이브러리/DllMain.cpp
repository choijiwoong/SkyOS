/*
DLL파일은 코어파일이 항상 0x100000에 로드되는 것과 달리 선호되는 번지가 고정되어있지 않다.
진입점 기본 세팅이 EntryPoint이기에 DllMain으로 실행시 진입점을 변경해줘야 한다.
 DLL로딩시 GRUB메뉴에  module /boot/SampleDll.dll "SAMPLE_DLL" 추가
일반적인 DLL과 달리 힙에 로드시켜 별다른 주소매핑이 필요하지 않지만 힙은 코드적재가아닌 데이터저장을 위한 공간이기에
WIN32에서 힙에 코드실행속성은 없다. 프로세스 공격을 이용한 형태로, 이러한 공격을 막기 위해 프로세스는 DLL코드가 정상적인
영역에 로드됐는지를 확인해야한다. 별도의 페이지 설정과 DLL을 별도의 가상주소에 매핑해야한다. 
*/
#include "ServiceFunc.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){ 
	//실제 WIN32 DLL은 프로세스 락을 호출(kEnterCriticalSection, kLeaveCriticalSection). SkyOS에서는 참조카운트만 증가(아키텍쳐 확장시 동기화 고려필수) 
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
	//모듈 로딩, 핸들반환 
	MODULE_HANDLE hwnd=SkyModuleManager::GetInstance()->LoadModuleFromMemory(moduleName);
	//모듈이 가지고있는 GetDLLInterface라는 메서드의 주소를 얻는다. (인터페이스를 통해) 
	PGetDLLInterface GetDLLInterface=(PGetDLLInterface)SkyModuleManager::GetInstance()->GetModuleFunction((MODULE_HANDLE)(hwnd), "GetDLLInterface");
	//해당 함수를 호출하여 DLL인터페이스를 얻는다? 
	const DLLInterface* dll_interface=GetDLLInterface();
	//인터페이스 내부가 궁금하긴한데, 인터페이스를 통해 함수 호출 및 결과 출력 
	int sum=dll_interface->AddNumber(5,6);
	SkyConsole::Print("AddNumber(5,6): %d\n", sum);
	//모듈 해제. 참조포인트만 내려줄듯 
	SkyModuleManager::GetInstance()->UnloadModule((MODULE_HANDLE)(hwnd));
}
