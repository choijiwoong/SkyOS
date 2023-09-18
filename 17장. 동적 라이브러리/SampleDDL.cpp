#include "DllMain.cpp"

int k=10;
int AddNumbers(int a, int b){
	return a+b+k;
}

typedef struct DLLInterface{
	int(*AddNumbers)(int a, int b);
}DLLInterface;

DLLInterface g_Interface={
	AddNumbers,
};

__declspec(dllexport) const DLLInterface* GetDLLInterface(){//__declspec(dllexport)로 PE파일의 export 디렉토리에 등록된다. 
	return &g_Interface;
}
