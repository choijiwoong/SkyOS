#include "try_catch(debugging).cpp"

jmp_buf pos;

void Func2(int x){
	if(x<5)
		longjmp(pos, x);//�������� ���� ���� �� ����� �������� ���·��� ����. 
	return;
} 

void Func1(){
	static int count=0;
	count++;
	Func2(count);
	return;
}

int main(){
	int result;
	result=setjmp(pos);//pos�� �������� ���� ���� 
	printf("%d\n", result);
	Func1();//longjmp��, �������� setjmp(pos)�� �̵�..����..��ƴ�. 
	printf("End!\n");
	
	return 0;
}
