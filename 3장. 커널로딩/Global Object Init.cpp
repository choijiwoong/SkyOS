//GRUB������ �۷ι� ��ü �� ���� ��ü�� ����� �� ����. (80K�ȿ� Ŀ�ο�Ʈ���� ���;��ϱ⿡) 
void _cdecl InitializeConstructors(){//�۷ι� �� ���� ������Ʈ �ʱ�ȭ �ڵ� 
	_atexit_init();
	_initterm(__xc_a, __xc_z);	
}

void __cdecl _initterm(_PVFV * pfbegin, _PVFV * pfend){//��ü�� ������ �ڵ� ���� 
	while(pfbegin<pfend){
		if(*pfbegin!=0)//�������� �ƴ϶�� 
			(**pfbegin)();//������ �Լ��� ���������� ȣ���Ѵ�. 
		++pfbegin;
	}
}

void _cdecl Exit(){//��ü �Ҹ��� �ڵ�(Ŀ�� ���� �� ���� ������Ʈ�� �Ҹ��� ȣ���� �ʼ��̴�) 
	while(cur_atexitlist_entries--){
		(*(--pf_atexitlist))();
	}
}
