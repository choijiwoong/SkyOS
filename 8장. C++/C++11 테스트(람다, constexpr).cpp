#include <iostream>

class TestInteger{
	public: 
		constexpr TestInteger(){}
		constexpr TestInteger(int value): m_Value(value){}
		constexpr operator int() const { return m_Value; }
	
	private:
		int m_Value=2;
};

void TestCPP11(){
	auto func=[x=5](){return x;};
	auto n1=func();
	printf("Lambda n1: %d\n", n1);
	
	constexpr TestInteger size(10);
	int x[size];
	x[3]=11;
	printf("constexpr x[3]: %d\n", x[3]);
}

int main(){
	TestCPP11();
	
	
	return 0;
}
