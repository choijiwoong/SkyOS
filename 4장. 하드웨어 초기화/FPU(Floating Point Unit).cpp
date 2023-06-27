bool InitFPU(){//제대로 완료되었으면 result에 1이 저장되고 정상적이면 true를,  FPU가 없다면 false의 값을 리턴. 
	int result=0;
	unsigned short temp;
	
	__asm{
		pushad; 모든 레지스터 스택에 저장
		mov eax, cr0
		and al, ~6; EM(Emulation_FPU가 내장되었는지)과 MP(Monitor Coprocessor_Task Switched로 1일 경우 FPU명령시 예외발생시킴)필드를 참고할 것인지)플래그 클리어. 1001을 &연산. eax=al+ar이다!
		mov cr0, eax; 마스킹 값을 저장
		fninit; FPU상태 초기화
		mov temp, 0x5A5A; FPU의 상태를 저장할 temp값 대충 초기화
		fnstsw temp; FPU의 상태(status)를 temp에 얻어온다
		cmp temp, 0; 값이 0이면 FPU가 없는거.
		jne noFPU
		fnstcw temp; FPU의 제어(control)값을 임시변수에 얻어오고 
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

bool EnableFPU(){//FPU활성화 코드
	#ifdef _WIN32
		unsigned long regCR4=__readcr4(); 각종 확장 기능을 제어하는 레지스터로 대체로 하드웨어 가상화 설정관리 
		__asm or regCR4, 0x200
		__writecr4(regCR4);
	#else //VS는 cr4레지스터를 지원하지 않기에 내장함수로 사용. 
	__asm{
		mov eax, cr4
		or eax, 0x200
		mov cr4, eax
	}
	#endif
} 

void TestFPU(){//제대로 실행이 되는지(FPU가 작동하는지) 
	float sampleFloat=0.3f;
	sampleFloat*=5.482f;
	SkyConsole::Print("sample Float value %f\n", sampleFloat);
}
