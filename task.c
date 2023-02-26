#include "task.h"
#include "string.h"
#include "memory.h"
#include "syscall.h"
#include "kernel.h"
#include "screen.h"

#define MAX_RUNNING_TASK 4
#define MAX_RUNNING_TICK (255 + 5)

volatile Task *gCurrentTaskAddr = nullptr;

static TSS *pTSS = nullptr;

static Queue *pReadyQueue = nullptr;

static Queue *pRunningQueue = nullptr;

static Queue *pWaitingQueue = nullptr;

static List *pSleepList = nullptr;

static List *pTaskPool = nullptr;

static TaskNode *pIdleTaskNode = nullptr;

static void IdleTask()
{
	while (true)
		;
}

static void TaskEntry()
{
	if (!IsEqual(gCurrentTaskAddr, nullptr))
	{
		gCurrentTaskAddr->tMain();
	}

	Exit(0);
}

static void InitTSS()
{
	pTSS = malloc(sizeof(TSS));

	SetDescValue(AddrOffset(gGdtInfo.entry, GDT_TSSIndex), (u32)(pTSS), sizeof(TSS) - 1, DA_386TSS + DA_DPL0);

	pTSS->ss0 = GDT_FlatModeDataSelector;
	pTSS->esp0 = BaseOfLoader;
	pTSS->iomb = sizeof(TSS);

	u16 ax = GDT_TssSelector;

	asm volatile(
		"movw %0, %%ax\n"
		"ltr      %%ax\n"
		:
		: "r"(ax)
		: "ax");
}

static void PrepareForRun(volatile Task *pTask)
{
	pTSS->esp0 = (u32)(StructOffset(pTask, Task, rv)) + sizeof(RegisterValue);

	SetDescValue(AddrOffset(gGdtInfo.entry, GDT_LDTIndex), (u32)(StructOffset(pTask, Task, ldt)), sizeof(pTask->ldt) - 1, DA_LDT + DA_DPL0);

	asm volatile(
		"movw %0, %%ax\n"
		"lldt     %%ax\n"
		:
		: "r"(pTask->ldtSelector)
		: "ax");
}

static void AppInfoToTaskDataStruct()
{
	pReadyQueue = malloc(sizeof(Queue));

	Queue_Init(pReadyQueue);

	pRunningQueue = malloc(sizeof(Queue));

	Queue_Init(pRunningQueue);

	pWaitingQueue = malloc(sizeof(Queue));

	Queue_Init(pWaitingQueue);

	pSleepList = malloc(sizeof(List));

	List_Init(pSleepList);

	pTaskPool = malloc(sizeof(List));

	List_Init(pTaskPool);
}

static TaskNode *AppInfoToTaskNode(AppInfo *appInfo)
{
	TaskNode *ret = malloc(sizeof(TaskNode));

	ret->task.stack = PMemAlloc(nullptr);

	SetDescValue(AddrOffset(ret->task.ldt, LDT_Code32Index), 0, 0xfffff, DA_32 + DA_C + DA_LIMIT_4K + DA_DPL3);
	SetDescValue(AddrOffset(ret->task.ldt, LDT_Data32Index), 0, 0xfffff, DA_32 + DA_DRWA + DA_LIMIT_4K + DA_DPL3);
	SetDescValue(AddrOffset(ret->task.ldt, LDT_Stack32Index), (u32)(ret->task.stack), PGEIndex((u32)(ret->task.stack) + PAGE_SIZE), DA_32 + DA_DRW + DA_LIMIT_4K + DA_DPL3);

	ret->task.rv.gs = GDT_UndefinedSelector;

	ret->task.rv.fs = LDT_Data32Selector;
	ret->task.rv.es = LDT_Data32Selector;
	ret->task.rv.ds = LDT_Data32Selector;

	ret->task.rv.ss = LDT_Stack32Selector;

	ret->task.rv.esp = (u32)(ret->task.stack) + PAGE_SIZE;

	ret->task.rv.cs = LDT_Code32Selector;
	ret->task.rv.eip = (u32)(TaskEntry);

	ret->task.rv.eflags = 0x3202;

	ret->task.ldtSelector = GDT_LdtSelector;

	ret->task.wait = malloc(sizeof(Queue));

	Queue_Init(ret->task.wait);

	ret->task.tMain = appInfo->tMain;

	ret->task.total = MAX_RUNNING_TICK - appInfo->priority;

	ret->task.name = IsEqual(appInfo->name, nullptr) ? nullptr : strdup(appInfo->name);

	return ret;
}

static bool CreateTaskToReady(AppInfo *appInfo)
{
	bool ret = (!IsEqual(appInfo, nullptr)) && (!IsEqual(appInfo->tMain, nullptr));

	if (ret)
	{
		TaskNode *taskNode = AppInfoToTaskNode(appInfo);

		List_Add(pTaskPool, StructOffset(taskNode, TaskNode, head.lHead));

		Queue_Add(pReadyQueue, StructOffset(taskNode, TaskNode, head.qHead));
	}

	return ret;
}

static void InitIdleTask()
{
	AppInfo appInfo = {"IdleTask", IdleTask, 255};

	pIdleTaskNode = AppInfoToTaskNode(&appInfo);
}

static void InitInitTask()
{
	AppInfo appInfo = {"InitInitTask", AMain, 255};

	CreateTaskToReady(&appInfo);
}

void TaskModuleInit()
{
	InitTSS();

	AppInfoToTaskDataStruct();

	InitIdleTask();

	InitInitTask();
}

static void ReadyToRunning()
{
	while ((Queue_Length(pRunningQueue) <= MAX_RUNNING_TASK) && (!IsEqual(Queue_Length(pReadyQueue), 0)))
	{
		TaskNode *taskNode = List_Node(Queue_Remove(pReadyQueue), TaskNode, head.qHead);

		taskNode->task.current = 0;

		Queue_Add(pRunningQueue, StructOffset(taskNode, TaskNode, head.qHead));
	}
}

static void RunningToReady()
{
	if (!IsEqual(gCurrentTaskAddr, StructOffset(pIdleTaskNode, TaskNode, task)))
	{
		if (IsEqual(++gCurrentTaskAddr->current, gCurrentTaskAddr->total))
		{
			Queue_Add(pReadyQueue, Queue_Remove(pRunningQueue));
		}
	}
}

static void SleepToReady()
{
	if (!List_IsEmpty(pSleepList))
	{
		ListNode *curListNode = pSleepList->next, *preListNode = nullptr;

		TaskNode *taskNode = nullptr;

		while (!IsEqual(curListNode, pSleepList))
		{
			taskNode = List_Node(curListNode, TaskNode, head.qHead);

			if (IsEqual(--taskNode->task.ticks, 0))
			{
				preListNode = curListNode->next;

				List_DelNode(curListNode);

				Queue_Add(pReadyQueue, StructOffset(taskNode, TaskNode, head.qHead));

				curListNode = preListNode;
			}
			else
			{
				curListNode = curListNode->next;
			}
		}
	}
}

void LaunchTask()
{
	if ((Queue_Length(pRunningQueue) <= MAX_RUNNING_TASK) && (!IsEqual(Queue_Length(pReadyQueue), 0)))
	{
		ReadyToRunning();
	}

	gCurrentTaskAddr = Queue_Length(pRunningQueue) > 0 ? StructOffset(List_Node(Queue_Front(pRunningQueue), TaskNode, head.qHead), TaskNode, task) : StructOffset(pIdleTaskNode, TaskNode, task);

	PrepareForRun(gCurrentTaskAddr);

	ClockInit();

	RunTask(gCurrentTaskAddr);
}

static void Schedule()
{
	if ((Queue_Length(pRunningQueue) <= MAX_RUNNING_TASK) && (!IsEqual(Queue_Length(pReadyQueue), 0)))
	{
		ReadyToRunning();
	}

	Task *pNextTask = Queue_Length(pRunningQueue) > 0 ? Queue_Rotate(pRunningQueue), StructOffset(List_Node(Queue_Front(pRunningQueue), TaskNode, head.qHead), TaskNode, task) : StructOffset(pIdleTaskNode, TaskNode, task);

	if (!IsEqual(pNextTask, gCurrentTaskAddr))
	{
		gCurrentTaskAddr = pNextTask;

		PrepareForRun(gCurrentTaskAddr);
	}
}

static void KillTask()
{
	TaskNode *pCurrentTask = List_Node(Queue_Remove(pRunningQueue), TaskNode, head.qHead);

	WaitToReady(pCurrentTask->task.wait);

	List_DelNode(StructOffset(pCurrentTask, TaskNode, head.lHead));

	PMemFree(pCurrentTask->task.stack);

	free(pCurrentTask->task.wait);

	free(pCurrentTask->task.name);

	free(pCurrentTask);

	Schedule();
}

static void WaitToReady(Queue *pWaitQueue)
{
	while (!IsEqual(Queue_Length(pWaitQueue), 0))
	{
		Queue_Add(pReadyQueue, Queue_Remove(pWaitQueue));
	}
}

static void RunningToWait(Queue *pWaitQueue)
{
	TaskNode *pCurrentTask = List_Node(Queue_Front(pRunningQueue), TaskNode, head.qHead);

	if (IsEqual(gCurrentTaskAddr, StructOffset(pCurrentTask, TaskNode, task)))
	{
		Queue_Add(pWaitQueue, Queue_Remove(pRunningQueue));
	}
}

static void RunningToSleep(u32 ms)
{
	u32 ticks = ms / jiffy;

	gCurrentTaskAddr->ticks = ticks > 0 ? ticks : 1;

	List_Add(pSleepList, Queue_Remove(pRunningQueue));
}

static bool FindTarget(ListNode *lhs, ListNode *rhs)
{
	return IsEqual(strcmp(List_Node(lhs, TaskNode, head.lHead)->task.name, List_Node(rhs, TaskNode, head.lHead)->task.name), 0);
}

static TaskNode *FindTaskByName(const char *name)
{
	TaskNode *ret = nullptr;

	if (!IsEqual(strcmp(name, pIdleTaskNode->task.name), 0))
	{
		TaskNode target;

		target.task.name = strdup(name);

		ret = List_Node(List_FindNode(pTaskPool, StructOffset(&target, TaskNode, head.lHead), FindTarget), TaskNode, head.lHead);

		free(target.task.name);
	}

	return ret;
}

static bool WaitTask(const char *name)
{
	TaskNode *taskNode = FindTaskByName(name);

	bool ret = (!IsEqual(taskNode, nullptr));

	if (ret)
	{
		RunningToWait(taskNode->task.wait);

		Schedule();
	}

	return ret;
}

static bool SleepTask(u32 ms)
{
	bool ret = !IsEqual(gCurrentTaskAddr, StructOffset(pIdleTaskNode, TaskNode, task));

	if (ret)
	{
		RunningToSleep(ms);

		Schedule();
	}

	return ret;
}

u32 TaskCallHandler(u32 cmd, u32 param1, u32 param2)
{
	u32 ret = -1;

	switch (cmd)
	{
	case SysCall_Task_Kill:
	{
		KillTask();
		break;
	}
	case SysCall_Task_Schedule:
	{
		SleepToReady();
		RunningToReady();
		Schedule();
		break;
	}
	case SysCall_Task_Register:
	{
		ret = IsEqual(CreateTaskToReady((void *)(param1)), false) ? 0 : 1;
		break;
	}
	case SysCall_Task_Wait:
	{
		WaitTask((void *)(param1));
		break;
	}
	case SysCall_Task_Sleep:
	{
		SleepTask(param1);
		break;
	}
	default:
		break;
	}

	return ret;
}