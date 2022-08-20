#include <task.h>
#include <memory.h>
#include <syscall.h>
#include <kernel.h>
#include <screen.h>

volatile Task* gCurrentTaskAddr = nullptr;

static TSS* pTSS = nullptr;

static Queue* pRunningQueue = nullptr;

static List* pTaskPool = nullptr;

static TaskNode* pIdleTaskNode = nullptr;

static void IdleTask()
{
	while(true){
		Delay(1);
	}
}

void TaskA()
{
	static u32 i = 0;

	while(true){

		SetPrintPos(0, 2);

		printf("This is TaskA : ");

		putchar('a' + i);

		i = (i + 1) % 26;

		Delay(1);
	}
}

void TaskB()
{
	static u32 i = 0;

	while(i < 8){

		SetPrintPos(0, 3);

		printf("This is TaskB : ");

		putchar('0' + i);

		i = (i + 1) % 10;

		Delay(1);
	}
}

static void TaskEntry()
{
	if(!IsEqual(gCurrentTaskAddr, nullptr)){
		gCurrentTaskAddr->tMain();
	}

	Exit();
}

static void InitTSS()
{
	pTSS = malloc(sizeof(TSS));

	SetDescValue(AddrOffset(gGdtInfo.entry, GDT_TSSIndex), (u32)(pTSS), sizeof(TSS) - 1, DA_386TSS + DA_DPL0);

	pTSS->ss0 = GDT_FlatModeDataSelector;
	pTSS->esp0 = BaseOfBoot;
	pTSS->iomb = sizeof(TSS);

	u16 ax = GDT_TssSelector;

	asm volatile(
		"movw %0, %%ax\n"
		"ltr      %%ax\n"
		:
		: "r"(ax)
		: "ax"
	);
}

static void PrepareForRun(volatile Task* pTask)
{
	pTSS->esp0 = (u32)(StructOffset(pTask, Task, rv)) + sizeof(RegisterValue);

	SetDescValue(AddrOffset(gGdtInfo.entry, GDT_LDTIndex), (u32)(StructOffset(pTask, Task, ldt)), sizeof(pTask->ldt) - 1, DA_LDT + DA_DPL0);

	asm volatile(
		"movw %0, %%ax\n"
		"lldt     %%ax\n"
		:
		: "r"(pTask->ldtSelector)
		: "ax"
	);
}

static void InitTaskDataStruct()
{
	pRunningQueue = malloc(sizeof(Queue));

	Queue_Init(pRunningQueue);

	pTaskPool = malloc(sizeof(List));

	List_Init(pTaskPool);
}

static void InitTask(Task* pTask, pFunc entry)
{
	pTask->stack = PMemAlloc(nullptr);

	SetDescValue(AddrOffset(pTask->ldt, LDT_Code32Index), 0, 0xfffff, DA_32 + DA_C + DA_LIMIT_4K + DA_DPL3);
	SetDescValue(AddrOffset(pTask->ldt, LDT_Data32Index), 0, 0xfffff, DA_32 + DA_DRWA + DA_LIMIT_4K + DA_DPL3);
	SetDescValue(AddrOffset(pTask->ldt, LDT_Stack32Index), (u32)(pTask->stack), PAGE_INDEX((u32)(pTask->stack) + PAGE_SIZE), DA_32 + DA_DRW + DA_LIMIT_4K + DA_DPL3);

	pTask->rv.gs = GDT_Video32Selector;

	pTask->rv.fs = LDT_Data32Selector;
	pTask->rv.es = LDT_Data32Selector;
	pTask->rv.ds = LDT_Data32Selector;

	pTask->rv.ss = LDT_Stack32Selector;

	pTask->rv.esp = (u32)(pTask->stack) + PAGE_SIZE;

	pTask->rv.cs = LDT_Code32Selector;
	pTask->rv.eip = (u32)(TaskEntry);

	pTask->rv.eflags = 0x3202;

	pTask->ldtSelector = GDT_LdtSelector;

	pTask->tMain = entry; 
}

static void CreateTask(pFunc entry)
{
	if(!IsEqual(entry, nullptr))
	{
		TaskNode* taskNode = malloc(sizeof(TaskNode));

		InitTask(StructOffset(taskNode, TaskNode, task), entry);

		List_Add(pTaskPool, StructOffset(taskNode, TaskNode, head.lHead));

		Queue_Add(pRunningQueue, StructOffset(taskNode, TaskNode, head.qHead));
	}
}

static void InitIdleTask()
{
	pIdleTaskNode = malloc(sizeof(TaskNode));

	InitTask(StructOffset(pIdleTaskNode, TaskNode, task), IdleTask);
}

void TaskModuleInit()
{
	InitTSS();

	InitTaskDataStruct();

	InitIdleTask();

	CreateTask(TaskA);	

	CreateTask(TaskB);
}

void LaunchTask()
{
	gCurrentTaskAddr = Queue_Length(pRunningQueue) > 0 ? StructOffset(List_Node(Queue_Front(pRunningQueue), TaskNode, head.qHead), TaskNode, task) : StructOffset(pIdleTaskNode, TaskNode, task);
	
	PrepareForRun(gCurrentTaskAddr);

	TimerInit();

	RunTask(gCurrentTaskAddr);
}

static void Schedule()
{
	Task* pNextTask = Queue_Length(pRunningQueue) > 0 ? Queue_Rotate(pRunningQueue), StructOffset(List_Node(Queue_Front(pRunningQueue), TaskNode, head.qHead), TaskNode, task) : StructOffset(pIdleTaskNode, TaskNode, task);

	if(!IsEqual(pNextTask, gCurrentTaskAddr))
	{
		gCurrentTaskAddr = pNextTask;

		PrepareForRun(gCurrentTaskAddr);
	}
}

static void KillTask()
{
	TaskNode* pCurrentTask = List_Node(Queue_Remove(pRunningQueue), TaskNode, head.qHead);

	List_DelNode(StructOffset(pCurrentTask, TaskNode, head.lHead));

	PMemFree(pCurrentTask->task.stack);

	free(pCurrentTask);

	Schedule();
}

static void PrintTaskInfo(u32 addr)
{
	RegisterValue* registerInfo = StructOffset(addr, Task, rv);
	ClearScreen();
	SetPrintPos(0, 0);
	printf("-------------------------------\n");
	printf("|=== Commom register value ===|\n");
	printf("| eax --> %p\n", registerInfo->eax);
	printf("| ebx --> %p\n", registerInfo->ebx);
	printf("| ecx --> %p\n", registerInfo->ecx);
	printf("| edx --> %p\n", registerInfo->edx);
	printf("| ebp --> %p\n", registerInfo->ebp);
	printf("| esi --> %p\n", registerInfo->esi);
	printf("| edi --> %p\n", registerInfo->edi);
	printf("|=== Segment register value ==|\n");
	printf("| ds --> 0b%b\n", registerInfo->ds);
	printf("| es --> 0b%b\n", registerInfo->es);
	printf("| fs --> 0b%b\n", registerInfo->fs);
	printf("| gs --> 0b%b\n", registerInfo->gs);
	printf("|==== Combin register value ==|\n");
	printf("| ss --> 0b%b\n", registerInfo->ss);
	printf("| esp --> %p\n", registerInfo->esp);
	printf("| eflags --> %p\n", registerInfo->eflags);
	printf("| cs --> 0b%b\n", registerInfo->cs);
	printf("| eip --> %p\n", registerInfo->eip);
	printf("|==== End register value =====|\n");
	printf("-------------------------------\n");
	while(true); // TODO
}

void TaskCallHandler(u32 cmd, u32 param1, u32 param2)
{
	switch(cmd){
		case 0 : {
			KillTask();
			break;
		}
		case 1 : {
			Schedule();
			break;
		}
		case 2 : {
			PrintTaskInfo(param1);
			break;
		}
		default: 
			break;
	}
}