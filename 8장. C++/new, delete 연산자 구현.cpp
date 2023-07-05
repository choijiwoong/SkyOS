#include <iostream>

void *operator new(size_t size){
	return (void*)kmalloc(size);
}
void * __cdecl operator new[](size_t size){//ȣ���ڿ� ���� ������ �����Ǳ⿡ �������� ó������ 
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
}//VS2015���ĺ��� deletep[]�� ���� ��������� �������� �ȴ�.

//0x09FFF0000 new: ���� �ƴ� ���� ������ placement�ּҿ��� �޸𸮸� ����ϰ� ���� ��
inline void *operator new(size_t, void *p);
inline void *operator new[](size_t, void *p);
inline void operator delete(void*, void*);
inline void operator delete[](void*, void*);

int main(){
	void *apic_address=reinterpret_cast<void*>(0x09FFF0000);
	APIC *apic=new (apic_address) APIC;//���ο� APIC�����͸� ����µ� void*�� �ּҸ� new�� ���ڷ� �ִ´�.
	apic->~apic();//�� ��� ��������� �Ҹ��ڸ� ȣ�����־�� �Ѵ�. 
}
