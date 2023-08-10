#include "ConsoleSystem.cpp"
/*
TSS(Task State Segment): �½�ũ ����Ī �� �����½�ũ���¸� �����ϴٰ� ������ �簳�Ǹ� �����Ͽ� ����ϴ� ������, ��� �������� ���� �����Ǵ� ������ TSS��� �Ѵ�. 
ESP�� g_esp��, gs fs es ds�� �����ϸ� ���ÿ� eip, cs, eflags, esp, ss�� ���ԵǴµ� �� �� IRETD����� ���� ���� �ڵ���� ���Ͱ� �����ϱ� TSS�� ������� �ʰ� ��Ƽ�½�ŷ ������ �����ϴ�. 
*/

void SwitchTask(int tick, register_t& registers);

void ISRHandler(registers_t regs){ 
	SwitchTask(_pit_ticks, regs);//���������� ���� 
}

//Ÿ�̸� ���ͷ�Ʈ �ڵ鷯
__declspec(naked) void InterruptPITHandler(){
	_asm{
		PUSHFD; //���� 
		cli
		
		pushad;//�����ͼ��׸�Ʈ Ǫ��, �Ŀ� ISRHandlerȣ��� ���Ǵµ� ������ �������� ���⿡ �� ����ġ�� �°� �� 
		push ds
		push es
		push fs
		push gs
		
		mov ax, 0x10; //Ŀ�ε����ͼ��׸�Ʈ ������ 
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		
		mov eax, esp; //���� esp�� ��� 
		mov g_esp, eax; //g_esp�� ���û��� register_t����ü�� ����Ų�� 
	}
	
	_pitTicks++;
	
	_asm{
		call ISRHandler; //Ÿ�̸� ���ͷ�Ʈ ó�� 
	}
	
	__asm{
		cmp g_pageDirectory, 0; //���ͷ�Ʈ�� ���������� �Ϸ�Ǿ��°�(������ ���丮 ���� 0�ΰ�) 
		jz pass
		
		//������ ���丮 ���� �����Ǿ��ִٸ�, ���������Ϳ� ������ ���丮�� �����Ͽ� ���ؽ�Ʈ ����Ī
		mov eax, g_esp
		mov esp, eas; //���������� �������� 
		
		mov eax, [g_pageDirectory]
		mov cr3, eax; //������ ���丮 �������� 
		
		pass: ;//���� 
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

//�½�ũ ����Ī ���� ����
bool Scheduler::DoSchedule(int tick, registers_t& registers){
	ProcessManager::TaskList* pTaskList=ProcessManager::GetInstance()->GetTaskList();
	
	int taskCount=pTaskList->size();
	if(taskCount==0)
		HaltSystem("Task Count Is 0\n");
	if(taskCount==1)//���ʿ� 
		return true;
		
	ProcessManager::TaskList::iterator iter=pTaskList->begin();//ù��° ����ũ�� �������� ����ũ�ε� 
	Thread* pThread=*iter;
	pThread->m_waitingTime--;
	if(pThread->m_waitingTime>0)//������ �ð��� ���� ���������� �½�ũ ����Ī�� �����Ų��(���Ѵ�) 
		return true;
		
	//����ð��� ����_�½�ũ ����Ī���� 
	pThread->m_taskState=TASK_STATE_WAIT;//������ 
	pThread->m_contextSnapshot=registers;//���� �������� ���¸� �����忡 ������
	pThread->m_esp=g_esp;//���� ����
	
	pTaskList->remove(pThread);//���ó���ϴ��� ���ʿ켱������ ���� 
	pTaskList->push_back(pThread); 
	
	Thread* pNextThread=pTaskList->front();//����Ī�� �½�ũ ������ 
	Process* pProcess=pNextThread->m_pParent;
	
	if(pNextThread->m_taskState==TASK_STATE_INIT){//ù �����̶�� 
		pNextThread->m_waitingTime=TASK_RUNNING_TIME;//������·� ���� 
		pNextThread->m_taskState=TASK_STATE_RUNNING;
		
		entryPoint=(int)pNextThread->frame.eip;//�½�ũ�� ��Ʈ������Ʈ, �����ּ�, �Ķ���� �������� 
		procStack=pNextThread->frame.esp;
		startParam=pNextThread->m_startParam;
		
		PageDirectory* pageDirectory=pNextThread->m_pParent->GetPageDirectory();
		VirtualMemoryManager::SetCurPageDirectory(pageDirectory);//�θ����μ����� ���������丮�� ���
		ProcessManager::GetInstance()->SetCurrentTask(pNextThread); //���� �۾��� ����Ī�� ������� ����
		{
			_asm{
				mov eax, [pageDirectory]
				mov cr3, eax; //������ ���丮 ���� 
				
				mov ecx, [entryPoint]; //��Ʈ�� ���� 
				mov esp, procStack; //���� ���� 
				xor ebp, ebp
				push ebp; //���̽� ������ �ʱ�ȭ �� Ǫ�� 
				mov ebx, [startParam]; //�Ķ���� ���� 
			}
			
			__asm{
				mov ax, 0x10; //������ ���׸�Ʈ �����͸� Ŀ�� ������ ���׸�Ʈ�� �ʱ�ȭ 
				mov ds, ax
				mov es, ax
				mov fs, ax
				mov gs, ax
				
				push ebx; //�Ķ���� 
				push 0; //ebp
				push 0x200; //EFLAGS
				push 0x08; //CS
				push ecx; //EIP
				
				mov al, 0x20; //���ͷ�Ʈ Ȱ��ȭ �� �� �½�ũ ���� 
				out 0x20, al; //ax������ ���?? 
				sti; //���ͷ�Ʈ �ο��̺� �÷��� �� 
				iretd
			}
		} 
	} else{ //�̹� �������̶�� 
		pNextThread->m_waitingTime=TASK_RUNNING_TIME;//������·� ���� 
		pNextThread->m_taskState=TASK_STATE_RUNNING;
		
		g_esp=pNextThread->m_esp;//�½�ũ�� ���°��� Ȱ��ȭ 
		g_pageDirectory=(uint32_t)pNextThread->GetPageDirectory();
		
		VirtualMemoryManager::SetCurPageDirectory( pNextThread->m_pParent->GetPageDirectory() );//�ش� �½�ũ�� ���� ���丮�� Ȱ��ȭ 
		ProcessManager::GetInstance()->SetCurrentTask( pNextThread );//���� �۾����� ���� 
	}
	
	return true;
}

void SwitchTask(int tick, register_t& registers){
	if(systemOn==false)
		return;
	Scheduler::GetInstance()->DoSchedule(tick, registers);
}
