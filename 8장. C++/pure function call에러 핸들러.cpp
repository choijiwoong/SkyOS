int __cdecl _ _purecall_handler(){//C++에서 순수 가상함수는 호출이 불가능하다. 
	//로그
	return 0; 
} 

struct PureCallBase{
	PureCallBase(){ mf(); }
	void mf(){
		pvf();//순수가상함수를 호출 
	}
	virtual void pvf()=0;
};
struct PureCallExtend: public PureCallBase{
	PureCallExtend(){}//생성시 pvf를 호출하는데, 아직 바로 밑줄의 pvf초기화가 수행되지 않았기에 순수가상함수오류 발생. 위의 핸들러 호출 
	virtual void pvf(){}//즉, 순수가상함수를 생성자에서 호출하면 안된다. 
};
