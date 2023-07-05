#include <iostream>

class Plane{
	public:
		Plane();
		virtual ~Plane();
		
		void SetX(int x){ m_x=x; }
		void SetY(int y){ m_y=y; }
		
		int GetX(){ return m_x; }
		int GetY(){ return m_y; }
		
		virtual bool IsRotate()=0;
		
	private:
		int m_x, m_y;
};

class ZetPlane: public Plane{
	public:
		ZetPlane();
		virtual ~ZetPlane();
		
		virtual bool IsRotate() override;
		
		int m_rotation;
};

void TestHeapCorruption(){
	SkyConsole::Print("\nHeap Corruption Test\n\n");
	
	Plane* pPlane=new Plane();
	*((char*)pPlane+sizeof(Plane)+1)=(char)0;//푸터영역의 값을 0으로 대입
	delete pPlane;//Heap Error!! footer->magic == HEAP_MAGIC 출력
	
	Plane* pPlane2=new Plane();
	memset(pPlane, 0, sizeof(Plane));//C++에서 가상메서드를 가지면 가상함수테이블이 생성되는데, memset으로 해당 테이블 포함 객체 전체를 초기화했기에 
	delete pPlane; //소멸자를 호출하는 과정에서 참조되어야하는 가상함수 테이블을 찾지 못해 에러를 발생시킨다. 
}

int main(){
	//1. 클래스 상속 테스트 
	Plane* pZetPlane=new ZetPlane();
	bool isRotate=pZetPlane->IsRotate();
	//pZetPlane->IsRotate();
	SkyConsole::Print("ZetPlane Address: 0x%x\n", pZetPlane);
	SkyConsole::Print("ZetPlane IsRotate(): %d\n", pZetPlane->IsRotate() ? 1 : 0);
	
	//2. 힙 손상 테스트
	TestHeapCorruption();
	
	//3. 클래스 상속 테스트 2
	int i=100;
	Plane* pPlane = new ZetPlane();
	pPlane->SetX(i);
	pPlane->SetY(i+5);
	
	pPlane->IsRotate();
	SkyConsole::Print("Plane X : %d, Plane Y : %d\n", pPlane->GetX(), pPlane->GetY());
	delete pPlane;
	
	return 0;
}
