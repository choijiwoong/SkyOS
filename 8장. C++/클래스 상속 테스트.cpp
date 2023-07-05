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
	*((char*)pPlane+sizeof(Plane)+1)=(char)0;//Ǫ�Ϳ����� ���� 0���� ����
	delete pPlane;//Heap Error!! footer->magic == HEAP_MAGIC ���
	
	Plane* pPlane2=new Plane();
	memset(pPlane, 0, sizeof(Plane));//C++���� ����޼��带 ������ �����Լ����̺��� �����Ǵµ�, memset���� �ش� ���̺� ���� ��ü ��ü�� �ʱ�ȭ�߱⿡ 
	delete pPlane; //�Ҹ��ڸ� ȣ���ϴ� �������� �����Ǿ���ϴ� �����Լ� ���̺��� ã�� ���� ������ �߻���Ų��. 
}

int main(){
	//1. Ŭ���� ��� �׽�Ʈ 
	Plane* pZetPlane=new ZetPlane();
	bool isRotate=pZetPlane->IsRotate();
	//pZetPlane->IsRotate();
	SkyConsole::Print("ZetPlane Address: 0x%x\n", pZetPlane);
	SkyConsole::Print("ZetPlane IsRotate(): %d\n", pZetPlane->IsRotate() ? 1 : 0);
	
	//2. �� �ջ� �׽�Ʈ
	TestHeapCorruption();
	
	//3. Ŭ���� ��� �׽�Ʈ 2
	int i=100;
	Plane* pPlane = new ZetPlane();
	pPlane->SetX(i);
	pPlane->SetY(i+5);
	
	pPlane->IsRotate();
	SkyConsole::Print("Plane X : %d, Plane Y : %d\n", pPlane->GetX(), pPlane->GetY());
	delete pPlane;
	
	return 0;
}
