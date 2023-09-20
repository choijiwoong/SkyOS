#include "SkyDebugger.cpp"

//DLL에서 사용할 파일입출력,메모리할당,화면출력 인터페이스인 SkyMockInterface 사용예시 

extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

typedef void (*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef I_MapFileReader*(*PGetDebugEngineDLL)();

void* GetModuleFunction(HINSTANCE handle, const char* func_name){
	return (void*)GetProcAddress(handle, func_name);
} 

int main(){
	HINSTANCE dllHandle=NULL;
	
	//디버그엔진 모듈 로드 
	dllHandle=LoadLibrary("DebugEngine.dll");
	char* fileName="SkyOS32.map";
	
	//디버그엔진모듈이 export하는 SetSkyMockInterface와 GetDebugEngineDLL함수를 얻어낸다. 
	PSetSkyMockInterface SetSkyMockInterface=(PSetSkyMockInterface)GetModuleFunction(dllHandle, "SetSkyMockInterface");
	PGetDebugEngineDLL GetDebugEngineDLLInterface=(PGetDebugEngineDLL)GetModuleFunction(dllHandle, "GetDebugEngineDLL");
	if(!GetDebugEngineDLLInterface || !SetSkyMockInterface){
		printf("GetDebugEngineDLL Aquired failed\n!");
		return 0;
	}
	
	//디버그엔진모듈에 인터페이스 제공 
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);
	
	//디버그엔진에서 맵 파일리더 인터페이스를 얻어온다. ? 말그대로 .map을 읽는 인터페이스인듯 
	I_MapFileReader* pMapReader=GetDebugEngineDLLInterface();
	if(pMapReader==nullptr){
		printf("MapReader Creation Fail!\n");
		return 0;
	}
	
	//디버그엔진에 맵파일을 읽어들인 후 
	pMapReader->readFile(fileName);
	pMapReader->setLoadAddress(0x100000);//MapReader의 주소를 SkyOs커널 로드되는 기준주소인 0x100000로 설정.
	
	int lineNumber=0;//가져올 데이터들 
	DWORD resultAddress=0;
	char function[256];
	char moduleName[256];
	char fileName2[256];
	
	//MapReader를 이용하여 주소에서 모듈이름, 함수이름등을 얻어낸다. 라인번호를 얻기위해선 COD파일이 필요하다? 
	int result=pMapReader->getAddressInfo(0x00100df0, moduleName, fileName2, lineNumber, function, resultAddress);
	
	return 0;
}
