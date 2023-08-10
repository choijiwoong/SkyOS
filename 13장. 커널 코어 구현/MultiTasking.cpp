#include "ConsoleSystem.cpp"
/*
TSS(Task State Segment): 태스크 스위칭 시 기존태스크상태를 저장하다가 실행이 재개되면 복구하여 사용하는 것으로, 모든 레지스터 값이 보존되는 영역을 TSS라고 한다. 
ESP를 g_esp로, gs fs es ds를 복원하면 스택에 eip, cs, eflags, esp, ss만 남게되는데 이 때 IRETD명령을 통해 원래 코드로의 복귀가 가능하기 TSS를 사용하지 않고 멀티태스킹 구현이 가능하다. 
*/

void SwitchTask(int tick, register_t& registers);

void ISRHandler(registers_t regs){ 
	SwitchTask(_pit_ticks, regs);//스택프레임 생성 
}

//타이머 인터럽트 핸들러
__declspec(naked) void InterruptPITHandler(){
	_asm{
		PUSHFD; //저장 
		cli
		
		pushad;//데이터세그먼트 푸시, 후에 ISRHandler호출시 사용되는데 스택은 역순으로 들어가기에 딱 제위치에 맞게 들어감 
		push ds
		push es
		push fs
		push gs
		
		mov ax, 0x10; //커널데이터세그먼트 가져옴 
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		
		mov eax, esp; //현재 esp값 백업 
		mov g_esp, eax; //g_esp는 스택상의 register_t구조체를 가리킨다 
	}
	
	_pitTicks++;
	
	_asm{
		call ISRHandler; //타이머 인터럽트 처리 
	}
	
	__asm{
		cmp g_pageDirectory, 0; //인터럽트가 정상적으로 완료되었는가(페이지 디렉토리 값이 0인가) 
		jz pass
		
		//페이지 디렉토리 값이 설정되어있다면, 스택포인터와 페이지 디렉토리를 변경하여 컨텍스트 스위칭
		mov eax, g_esp
		mov esp, eas; //스택포인터 도르마무 
		
		mov eax, [g_pageDirectory]
		mov cr3, eax; //페이지 디렉토리 도르마무 
		
		pass: ;//복원 
		pop gs
		pop fs
		pop es
		pop ds
		
		popad;
		
		mov al, 0x20
		out 0x20, al
		POPFD
		iretd
	}
}

//태스크 스위칭 세부 구현
bool Scheduler::DoSchedule(int tick, registers_t& registers){
	ProcessManager::TaskList* pTaskList=ProcessManager::GetInstance()->GetTaskList();
	
	int taskCount=pTaskList->size();
	if(taskCount==0)
		HaltSystem("Task Count Is 0\n");
	if(taskCount==1)//불필요 
		return true;
		
	ProcessManager::TaskList::iterator iter=pTaskList->begin();//첫번째 리스크가 수행중인 리스크인데 
	Thread* pThread=*iter;
	pThread->m_waitingTime--;
	if(pThread->m_waitingTime>0)//수행할 시간이 아직 남아있으면 태스크 스위칭을 종료시킨다(안한다) 
		return true;
		
	//수행시간이 만료_태스크 스위칭시작 
	pThread->m_taskState=TASK_STATE_WAIT;//대기상태 
	pThread->m_contextSnapshot=registers;//현재 레지스터 상태를 스레드에 스냅샷
	pThread->m_esp=g_esp;//스택 저장
	
	pTaskList->remove(pThread);//방금처리하던거 뒤쪽우선순위로 넣음 
	pTaskList->push_back(pThread); 
	
	Thread* pNextThread=pTaskList->front();//스위칭할 태스크 가져옴 
	Process* pProcess=pNextThread->m_pParent;
	
	if(pNextThread->m_taskState==TASK_STATE_INIT){//첫 실행이라면 
		pNextThread->m_waitingTime=TASK_RUNNING_TIME;//실행상태로 변경 
		pNextThread->m_taskState=TASK_STATE_RUNNING;
		
		entryPoint=(int)pNextThread->frame.eip;//태스크의 엔트리포인트, 스택주소, 파라미터 가져오기 
		procStack=pNextThread->frame.esp;
		startParam=pNextThread->m_startParam;
		
		PageDirectory* pageDirectory=pNextThread->m_pParent->GetPageDirectory();
		VirtualMemoryManager::SetCurPageDirectory(pageDirectory);//부모프로세스의 페이지디렉토리를 사용
		ProcessManager::GetInstance()->SetCurrentTask(pNextThread); //현재 작업을 스위칭한 스레드로 설정
		{
			_asm{
				mov eax, [pageDirectory]
				mov cr3, eax; //페이지 디렉토리 설정 
				
				mov ecx, [entryPoint]; //엔트리 설정 
				mov esp, procStack; //스택 설정 
				xor ebp, ebp
				push ebp; //베이스 포인터 초기화 후 푸시 
				mov ebx, [startParam]; //파라미터 설정 
			}
			
			__asm{
				mov ax, 0x10; //데스터 세그먼트 셀렉터를 커널 데이터 세그먼트로 초기화 
				mov ds, ax
				mov es, ax
				mov fs, ax
				mov gs, ax
				
				push ebx; //파라미터 
				push 0; //ebp
				push 0x200; //EFLAGS
				push 0x08; //CS
				push ecx; //EIP
				
				mov al, 0x20; //인터럽트 활성화 및 새 태스크 실행 
				out 0x20, al; //ax데이터 출력?? 
				sti; //인터럽트 인에이블 플래그 셋 
				iretd
			}
		} 
	} else{ //이미 실행중이라면 
		pNextThread->m_waitingTime=TASK_RUNNING_TIME;//실행상태로 변경 
		pNextThread->m_taskState=TASK_STATE_RUNNING;
		
		g_esp=pNextThread->m_esp;//태스크의 상태값을 활성화 
		g_pageDirectory=(uint32_t)pNextThread->GetPageDirectory();
		
		VirtualMemoryManager::SetCurPageDirectory( pNextThread->m_pParent->GetPageDirectory() );//해당 태스크가 가진 디렉토리를 활성화 
		ProcessManager::GetInstance()->SetCurrentTask( pNextThread );//현재 작업으로 설정 
	}
	
	return true;
}

void SwitchTask(int tick, register_t& registers){
	if(systemOn==false)
		return;
	Scheduler::GetInstance()->DoSchedule(tick, registers);
}
