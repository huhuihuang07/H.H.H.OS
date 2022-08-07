#include <task.h>
#include <syscall.h>
#include <kernel.h>
#include <screen.h>

#define MAX_RUNNING_TASK 10

volatile Task* gCurrentTaskAddr = nullptr;

static TSS gTSS = {0};

static TaskNode gTaskNodeBuffer[MAX_RUNNING_TASK] = {0};

static Queue gRunningQueue = {0};

static Queue* pRunningQueue = &gRunningQueue;

static Stack gTaskPool = {0};

static Stack* pTaskPool = &gTaskPool;

static TaskNode gIdleTaskNode = {0};

static TaskNode* pIdleTaskNode = &gIdleTaskNode;

static void IdleTask()
{
	while(true)
	{
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

	while(true){

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

	u32 ax = 0;

	SysCall(ax);
}

static void InitTSS()
{
	SetDescValue(AddrOffset(gGdtInfo.entry, GDT_TSSIndex), (u32)(&gTSS), sizeof(TSS) - 1, DA_386TSS + DA_DPL0);

	gTSS.ss0 = GDT_FlatModeDataSelector;
	gTSS.esp0 = BaseOfBoot;
	gTSS.iomb = sizeof(TSS);

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
	gTSS.esp0 = (u32)(StructOffset(pTask, Task, rv)) + sizeof(RegisterValue);

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
	Queue_Init(pRunningQueue);

	Stack_Init(pTaskPool);

	for(u32 i = 0; i != MAX_RUNNING_TASK; ++i)
	{
		Stack_Push(pTaskPool, StructOffset(AddrOffset(gTaskNodeBuffer, i), TaskNode, head.sHead));
	}
}

static void InitTask(Task* pTask, pFunc entry)
{
	SetDescValue(AddrOffset(pTask->ldt, LDT_Code32Index), 0, 0xfffff, DA_32 + DA_C + DA_LIMIT_4K + DA_DPL3);
	SetDescValue(AddrOffset(pTask->ldt, LDT_Data32Index), 0, 0xfffff, DA_32 + DA_DRWA + DA_LIMIT_4K + DA_DPL3);
	SetDescValue(AddrOffset(pTask->ldt, LDT_Stack32Index), (u32)(StructOffset(pTask, Task, stack)), (u32)(StructOffset(pTask, Task, stack)) + sizeof(pTask->stack), DA_32 + DA_DRW + DA_DPL3);

	pTask->rv.gs = GDT_Video32Selector;

	pTask->rv.fs = LDT_Data32Selector;
	pTask->rv.es = LDT_Data32Selector;
	pTask->rv.ds = LDT_Data32Selector;

	pTask->rv.ss = LDT_Stack32Selector;

	pTask->rv.esp = (u32)(StructOffset(pTask, Task, stack)) + sizeof(pTask->stack);

	pTask->rv.cs = LDT_Code32Selector;
	pTask->rv.eip = (u32)(TaskEntry);

	pTask->rv.eflags = 0x3202;

	pTask->ldtSelector = GDT_LdtSelector;

	pTask->tMain = entry; 
}

static void CreateTask(pFunc entry)
{
	if(IsEqual(Stack_IsEmpty(pTaskPool), false))
	{
		TaskNode* taskNode = List_Node(Stack_Top(pTaskPool), TaskNode, head.sHead);

		Stack_Pop(pTaskPool);

		InitTask(StructOffset(taskNode, TaskNode, task), entry);

		Queue_Add(pRunningQueue, StructOffset(taskNode, TaskNode, head.qHead));
	}
}

void TaskModuleInit()
{
	InitTSS();

	InitTaskDataStruct();

	InitTask(StructOffset(pIdleTaskNode, TaskNode, task), IdleTask);

	// CreateTask(TaskA);	

	// CreateTask(TaskB);
}

void LaunchTask()
{
	if(Queue_Length(pRunningQueue) > 0)
	{
		gCurrentTaskAddr = StructOffset(List_Node(Queue_Front(pRunningQueue), TaskNode, head.qHead), TaskNode, task);
	}else{
		gCurrentTaskAddr = StructOffset(pIdleTaskNode, TaskNode, task);
	}
	
	PrepareForRun(gCurrentTaskAddr);

	TimerInit();

	RunTask(gCurrentTaskAddr);
}

void Schedule()
{
	Task* pNextTask = nullptr;

	if(Queue_Length(pRunningQueue) > 0)
	{
		Queue_Rotate(pRunningQueue);

		pNextTask = StructOffset(List_Node(Queue_Front(pRunningQueue), TaskNode, head.qHead), TaskNode, task);	
	}else{
		pNextTask = StructOffset(pIdleTaskNode, TaskNode, task);
	}
	
	if(!IsEqual(pNextTask, gCurrentTaskAddr)){
		PrepareForRun(gCurrentTaskAddr);
	}
}

void KillTask()
{
	Stack_Push(pTaskPool, StructOffset(List_Node(Queue_Remove(pRunningQueue), TaskNode, head.qHead), TaskNode, head.sHead));

	Schedule();
}