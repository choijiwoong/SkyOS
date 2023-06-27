bool InitFPU(){//����� �Ϸ�Ǿ����� result�� 1�� ����ǰ� �������̸� true��,  FPU�� ���ٸ� false�� ���� ����. 
	int result=0;
	unsigned short temp;
	
	__asm{
		pushad; ��� �������� ���ÿ� ����
		mov eax, cr0
		and al, ~6; EM(Emulation_FPU�� ����Ǿ�����)�� MP(Monitor Coprocessor_Task Switched�� 1�� ��� FPU��ɽ� ���ܹ߻���Ŵ)�ʵ带 ������ ������)�÷��� Ŭ����. 1001�� &����. eax=al+ar�̴�!
		mov cr0, eax; ����ŷ ���� ����
		fninit; FPU���� �ʱ�ȭ
		mov temp, 0x5A5A; FPU�� ���¸� ������ temp�� ���� �ʱ�ȭ
		fnstsw temp; FPU�� ����(status)�� temp�� ���´�
		cmp temp, 0; ���� 0�̸� FPU�� ���°�.
		jne noFPU
		fnstcw temp; FPU�� ����(control)���� �ӽú����� ������ 
		mov ax, temp
		and ax, 0x103F
		cmp ax, 0x003F
		jne result, 1
		mov result, 1
		noFPU:
			popad
	}
	
	return result==1;
}

bool EnableFPU(){//FPUȰ��ȭ �ڵ�
	#ifdef _WIN32
		unsigned long regCR4=__readcr4(); ���� Ȯ�� ����� �����ϴ� �������ͷ� ��ü�� �ϵ���� ����ȭ �������� 
		__asm or regCR4, 0x200
		__writecr4(regCR4);
	#else //VS�� cr4�������͸� �������� �ʱ⿡ �����Լ��� ���. 
	__asm{
		mov eax, cr4
		or eax, 0x200
		mov cr4, eax
	}
	#endif
} 

void TestFPU(){//����� ������ �Ǵ���(FPU�� �۵��ϴ���) 
	float sampleFloat=0.3f;
	sampleFloat*=5.482f;
	SkyConsole::Print("sample Float value %f\n", sampleFloat);
}
