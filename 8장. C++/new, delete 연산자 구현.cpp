#include <iostream>

void *operator new(size_t size){
	return (void*)kmalloc(size);
}
void * __cdecl operator new[](size_t size){//호출자에 의해 스택이 정리되기에 가변인자 처리가능 
	return (void*)kmalloc(size);
}

void operator delete(void *p, size_t size){
	kfree(p);
}
void __cdecl operator delete(void *p, size_t size){
	kfree(p);
}
void operator delete[](void *p){
	kfree(p);
}//VS2015이후부터 deletep[]를 직접 구현해줘야 컴파일이 된다.

//0x09FFF0000 new: 힙이 아닌 직접 지정한 placement주소에서 메모리를 사용하고 싶을 때
inline void *operator new(size_t, void *p);
inline void *operator new[](size_t, void *p);
inline void operator delete(void*, void*);
inline void operator delete[](void*, void*);

int main(){
	void *apic_address=reinterpret_cast<void*>(0x09FFF0000);
	APIC *apic=new (apic_address) APIC;//새로운 APIC포인터를 만드는데 void*형 주소를 new의 인자로 넣는다.
	apic->~apic();//이 경우 명시적으로 소멸자를 호출해주어야 한다. 
}
