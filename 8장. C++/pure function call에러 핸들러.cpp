int __cdecl _ _purecall_handler(){//C++���� ���� �����Լ��� ȣ���� �Ұ����ϴ�. 
	//�α�
	return 0; 
} 

struct PureCallBase{
	PureCallBase(){ mf(); }
	void mf(){
		pvf();//���������Լ��� ȣ�� 
	}
	virtual void pvf()=0;
};
struct PureCallExtend: public PureCallBase{
	PureCallExtend(){}//������ pvf�� ȣ���ϴµ�, ���� �ٷ� ������ pvf�ʱ�ȭ�� ������� �ʾұ⿡ ���������Լ����� �߻�. ���� �ڵ鷯 ȣ�� 
	virtual void pvf(){}//��, ���������Լ��� �����ڿ��� ȣ���ϸ� �ȵȴ�. 
};
