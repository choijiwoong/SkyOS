#include "try_catch(debugging).cpp"

jmp_buf pos;

void Func2(int x){
	if(x<5)
		longjmp(pos, x);//레지스터 정보 복원 및 저장된 레지스터 상태로의 복원. 
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
	result=setjmp(pos);//pos에 레지스터 정보 저장 
	printf("%d\n", result);
	Func1();//longjmp시, 위윗줄의 setjmp(pos)로 이동..ㅎㄷ..어렵다. 
	printf("End!\n");
	
	return 0;
}
