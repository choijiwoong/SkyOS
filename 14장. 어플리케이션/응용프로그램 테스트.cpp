#include "UserAPI.h"
#include "string.h"

int main(){//EntryPoint함수가 main을 호출 
	char* message="Hello world!!\n";
	printf(message);//커널이 제공하는 시스템 API 
	
	char* reply=new char[100];//힙이 할당 
	strcpy(reply, "Process Reply\n");//커널의 공통 라이브러리를 유저프로세스에서 활용 
	
	printf(reply);
	delete reply;
	return 0;
}
