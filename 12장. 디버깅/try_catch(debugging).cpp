__declspec(naked) int setjmp(jmp_buf env){
	__asm{
		mov edx, 4[esp]	//jmp_buf 버퍼포인터_느낌이 매개변수 크기가 4byte이고, 매개변수 env를 가져와서, 해당 자리에 레지스터 상태값 백업하는듯 
		mov eax, [esp]	//함수복귀주소
		
		mov OFS_EIP[edx], eax//레지스터를 버퍼에 저장 
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
		mov edx, 4[esp]	//jmp_buf 버퍼포인터_백업된 env가져오기 
		mov eax, 8[esp]	//value저장_돌아갈 위치를 가져오기(ret으로 이동될) 
		
		mov esp, OFS_ESP[edx]
		mov ebx, OFS_EIP[edx]
		mov [esp], ebx			//?원래 값을 돌려놓는건가... 
		mov ebp, OFS_EBP[edx]
		mov ebx, OFS_EBX[edx]
		mov edi, OFS_EDI[edx]
		mov esi, OFS_ESI[edx] 
		ret//수정된 함수 복귀주소로 이동(setjmp호출 다음위치) 
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
