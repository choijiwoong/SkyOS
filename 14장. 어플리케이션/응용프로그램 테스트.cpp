#include "UserAPI.h"
#include "string.h"

int main(){//EntryPoint�Լ��� main�� ȣ�� 
	char* message="Hello world!!\n";
	printf(message);//Ŀ���� �����ϴ� �ý��� API 
	
	char* reply=new char[100];//���� �Ҵ� 
	strcpy(reply, "Process Reply\n");//Ŀ���� ���� ���̺귯���� �������μ������� Ȱ�� 
	
	printf(reply);
	delete reply;
	return 0;
}
