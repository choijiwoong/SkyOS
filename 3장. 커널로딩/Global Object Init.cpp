//GRUB에서는 글로벌 객체 및 정적 객체를 사용할 수 없다. (80K안에 커널엔트리가 나와야하기에) 
void _cdecl InitializeConstructors(){//글로벌 및 정적 오브젝트 초기화 코드 
	_atexit_init();
	_initterm(__xc_a, __xc_z);	
}

void __cdecl _initterm(_PVFV * pfbegin, _PVFV * pfend){//객체의 생성자 코드 실행 
	while(pfbegin<pfend){
		if(*pfbegin!=0)//마지막이 아니라면 
			(**pfbegin)();//생성자 함수를 순차적으로 호출한다. 
		++pfbegin;
	}
}

void _cdecl Exit(){//객체 소멸자 코드(커널 종료 시 동적 오브젝트의 소멸자 호출이 필수이다) 
	while(cur_atexitlist_entries--){
		(*(--pf_atexitlist))();
	}
}
