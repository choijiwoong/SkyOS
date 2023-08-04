__declspec(naked) int setjmp(jmp_buf env){
	__asm{
		mov edx, 4[esp]	//jmp_buf ����������_������ �Ű����� ũ�Ⱑ 4byte�̰�, �Ű����� env�� �����ͼ�, �ش� �ڸ��� �������� ���°� ����ϴµ� 
		mov eax, [esp]	//�Լ������ּ�
		
		mov OFS_EIP[edx], eax//�������͸� ���ۿ� ���� 
		mov OFS_EBP[edx], ebp
		mov OFS_EBX[edx], ebx
		mov OFS_EDI[edx], ebi
		mov OFS_ESI[edx], esi
		mov OFS_ESP[edx], esp
		xor eax, eax
		ret 
	}
}

__declspec(naked) void longjmp(jmp_buf env, int value){
	__asm{
		mov edx, 4[esp]	//jmp_buf ����������_����� env�������� 
		mov eax, 8[esp]	//value����_���ư� ��ġ�� ��������(ret���� �̵���) 
		
		mov esp, OFS_ESP[edx]
		mov ebx, OFS_EIP[edx]
		mov [esp], ebx			//?���� ���� �������°ǰ�... 
		mov ebp, OFS_EBP[edx]
		mov ebx, OFS_EBX[edx]
		mov edi, OFS_EDI[edx]
		mov esi, OFS_ESI[edx] 
		ret//������ �Լ� �����ּҷ� �̵�(setjmpȣ�� ������ġ) 
	}
}

//try-catch test
void throwArgumentException(){
	//puts("Function reached");
	throw(ArgumentException, (char*)"Ooh! Some Argument eas thrown.");
}

void TestTryCatch(){
	try{
		thrownArgumentException();
	} catch(ArgumentException){
		if(__ctrycatch_exception_message_exists)
			SkyConsole::Print("message: %s\n", __ctrycatch_exception_message);
	} finally{
		//puts("finally block reached");
		SkyConsole::Print("Finally!\n");
	}
}
