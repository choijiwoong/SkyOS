#include "SkyDebugger.cpp"

//DLL���� ����� ���������,�޸��Ҵ�,ȭ����� �������̽��� SkyMockInterface ��뿹�� 

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
	
	//����׿��� ��� �ε� 
	dllHandle=LoadLibrary("DebugEngine.dll");
	char* fileName="SkyOS32.map";
	
	//����׿�������� export�ϴ� SetSkyMockInterface�� GetDebugEngineDLL�Լ��� ����. 
	PSetSkyMockInterface SetSkyMockInterface=(PSetSkyMockInterface)GetModuleFunction(dllHandle, "SetSkyMockInterface");
	PGetDebugEngineDLL GetDebugEngineDLLInterface=(PGetDebugEngineDLL)GetModuleFunction(dllHandle, "GetDebugEngineDLL");
	if(!GetDebugEngineDLLInterface || !SetSkyMockInterface){
		printf("GetDebugEngineDLL Aquired failed\n!");
		return 0;
	}
	
	//����׿�����⿡ �������̽� ���� 
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);
	
	//����׿������� �� ���ϸ��� �������̽��� ���´�. ? ���״�� .map�� �д� �������̽��ε� 
	I_MapFileReader* pMapReader=GetDebugEngineDLLInterface();
	if(pMapReader==nullptr){
		printf("MapReader Creation Fail!\n");
		return 0;
	}
	
	//����׿����� �������� �о���� �� 
	pMapReader->readFile(fileName);
	pMapReader->setLoadAddress(0x100000);//MapReader�� �ּҸ� SkyOsĿ�� �ε�Ǵ� �����ּ��� 0x100000�� ����.
	
	int lineNumber=0;//������ �����͵� 
	DWORD resultAddress=0;
	char function[256];
	char moduleName[256];
	char fileName2[256];
	
	//MapReader�� �̿��Ͽ� �ּҿ��� ����̸�, �Լ��̸����� ����. ���ι�ȣ�� ������ؼ� COD������ �ʿ��ϴ�? 
	int result=pMapReader->getAddressInfo(0x00100df0, moduleName, fileName2, lineNumber, function, resultAddress);
	
	return 0;
}
