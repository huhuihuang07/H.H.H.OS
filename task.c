#include "task.h"
#include "string.h"
#include "memory.h"
#include "syscall.h"
#include "kernel.h"
#include "screen.h"

#define MAX_RUNNING_TASK 4
#define MAX_RUNNING_TICK (255 + 5)

volatile Task_t* gCurrentTaskAddr = nullptr;

static TSS_t* pTSS = nullptr;

static Queue_t* pReadyQueue = nullptr;

static Queue_t* pRunningQueue = nullptr;

static Queue_t* pWaitingQueue = nullptr;

static List* pSleepList = nullptr;

static List* pTaskPool = nullptr;

static TaskNode_t* pIdleTaskNode = nullptr;

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
    pTSS = malloc(sizeof(TSS_t));

    SetDescValue(AddrOffset(gGdtInfo.entry, GDT_TSSIndex), (uint32_t)(pTSS), sizeof(TSS_t) - 1, DA_386TSS + DA_DPL0);

    pTSS->ss0  = GDT_FlatModeDataSelector;
    pTSS->esp0 = BaseOfLoader;
    pTSS->iomb = sizeof(TSS_t);

    uint16_t ax = GDT_TssSelector;

    asm volatile(
        "movw %0, %%ax\n"
        "ltr      %%ax\n"
        :
        : "r"(ax)
        : "ax");
}

static void PrepareForRun(volatile Task_t* pTask)
{
    pTSS->esp0 = (uint32_t)(StructOffset(pTask, Task_t, rv)) + sizeof(RegisterValue_t);

    SetDescValue(AddrOffset(gGdtInfo.entry, GDT_LDTIndex), (uint32_t)(StructOffset(pTask, Task_t, ldt)), sizeof(pTask->ldt) - 1, DA_LDT + DA_DPL0);

    asm volatile(
        "movw %0, %%ax\n"
        "lldt     %%ax\n"
        :
        : "r"(pTask->ldtSelector)
        : "ax");
}

static void AppInfoToTaskDataStruct()
{
    pReadyQueue = malloc(sizeof(Queue_t));

    Queue_Init(pReadyQueue);

    pRunningQueue = malloc(sizeof(Queue_t));

    Queue_Init(pRunningQueue);

    pWaitingQueue = malloc(sizeof(Queue_t));

    Queue_Init(pWaitingQueue);

    pSleepList = malloc(sizeof(List));

    List_Init(pSleepList);

    pTaskPool = malloc(sizeof(List));

    List_Init(pTaskPool);
}

static TaskNode_t* AppInfoToTaskNode(AppInfo* appInfo)
{
    TaskNode_t* ret = malloc(sizeof(TaskNode_t));

    ret->task.stack = PMemAlloc(nullptr);

    SetDescValue(AddrOffset(ret->task.ldt, LDT_Code32Index), 0, 0xfffff, DA_32 + DA_C + DA_LIMIT_4K + DA_DPL3);
    SetDescValue(AddrOffset(ret->task.ldt, LDT_Data32Index), 0, 0xfffff, DA_32 + DA_DRWA + DA_LIMIT_4K + DA_DPL3);
    SetDescValue(AddrOffset(ret->task.ldt, LDT_Stack32Index), (uint32_t)(ret->task.stack), PGEIndex((uint32_t)(ret->task.stack) + PAGE_SIZE), DA_32 + DA_DRW + DA_LIMIT_4K + DA_DPL3);

    ret->task.rv.gs = GDT_UndefinedSelector;

    ret->task.rv.fs = LDT_Data32Selector;
    ret->task.rv.es = LDT_Data32Selector;
    ret->task.rv.ds = LDT_Data32Selector;

    ret->task.rv.ss = LDT_Stack32Selector;

    ret->task.rv.esp = (uint32_t)(ret->task.stack) + PAGE_SIZE;

    ret->task.rv.cs  = LDT_Code32Selector;
    ret->task.rv.eip = (uint32_t)(TaskEntry);

    ret->task.rv.eflags = 0x3202;

    ret->task.ldtSelector = GDT_LdtSelector;

    ret->task.wait = malloc(sizeof(Queue_t));

    Queue_Init(ret->task.wait);

    ret->task.tMain = appInfo->tMain;

    ret->task.total = MAX_RUNNING_TICK - appInfo->priority;

    ret->task.name = IsEqual(appInfo->name, nullptr) ? nullptr : strdup(appInfo->name);

    return ret;
}

static bool CreateTaskToReady(AppInfo* appInfo)
{
    bool ret = (!IsEqual(appInfo, nullptr)) && (!IsEqual(appInfo->tMain, nullptr));

    if (ret)
    {
        TaskNode_t* taskNode = AppInfoToTaskNode(appInfo);

        List_Add(pTaskPool, StructOffset(taskNode, TaskNode_t, head.lHead));

        Queue_Add(pReadyQueue, StructOffset(taskNode, TaskNode_t, head.qHead));
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
        TaskNode_t* taskNode = List_Node(Queue_Remove(pReadyQueue), TaskNode_t, head.qHead);

        taskNode->task.current = 0;

        Queue_Add(pRunningQueue, StructOffset(taskNode, TaskNode_t, head.qHead));
    }
}

static void RunningToReady()
{
    if (!IsEqual(gCurrentTaskAddr, StructOffset(pIdleTaskNode, TaskNode_t, task)))
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
        ListNode_t *curListNode = pSleepList->next, *preListNode = nullptr;

        TaskNode_t* taskNode = nullptr;

        while (!IsEqual(curListNode, pSleepList))
        {
            taskNode = List_Node(curListNode, TaskNode_t, head.qHead);

            if (IsEqual(--taskNode->task.ticks, 0))
            {
                preListNode = curListNode->next;

                List_DelNode(curListNode);

                Queue_Add(pReadyQueue, StructOffset(taskNode, TaskNode_t, head.qHead));

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

    gCurrentTaskAddr = Queue_Length(pRunningQueue) > 0 ? StructOffset(List_Node(Queue_Front(pRunningQueue), TaskNode_t, head.qHead), TaskNode_t, task) : StructOffset(pIdleTaskNode, TaskNode_t, task);

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

    Task_t *pNextTask = Queue_Length(pRunningQueue) > 0 ? Queue_Rotate(pRunningQueue), StructOffset(List_Node(Queue_Front(pRunningQueue), TaskNode_t, head.qHead), TaskNode_t, task) : StructOffset(pIdleTaskNode, TaskNode_t, task);

    if (!IsEqual(pNextTask, gCurrentTaskAddr))
    {
        gCurrentTaskAddr = pNextTask;

        PrepareForRun(gCurrentTaskAddr);
    }
}

static void KillTask()
{
    TaskNode_t* pCurrentTask = List_Node(Queue_Remove(pRunningQueue), TaskNode_t, head.qHead);

    WaitToReady(pCurrentTask->task.wait);

    List_DelNode(StructOffset(pCurrentTask, TaskNode_t, head.lHead));

    PMemFree(pCurrentTask->task.stack);

    free(pCurrentTask->task.wait);

    free(pCurrentTask->task.name);

    free(pCurrentTask);

    Schedule();
}

static void WaitToReady(Queue_t* pWaitQueue)
{
    while (!IsEqual(Queue_Length(pWaitQueue), 0))
    {
        Queue_Add(pReadyQueue, Queue_Remove(pWaitQueue));
    }
}

static void RunningToWait(Queue_t* pWaitQueue)
{
    TaskNode_t* pCurrentTask = List_Node(Queue_Front(pRunningQueue), TaskNode_t, head.qHead);

    if (IsEqual(gCurrentTaskAddr, StructOffset(pCurrentTask, TaskNode_t, task)))
    {
        Queue_Add(pWaitQueue, Queue_Remove(pRunningQueue));
    }
}

static void RunningToSleep(uint32_t ms)
{
    uint32_t ticks = ms / jiffy;

    gCurrentTaskAddr->ticks = ticks > 0 ? ticks : 1;

    List_Add(pSleepList, Queue_Remove(pRunningQueue));
}

static bool FindTarget(ListNode_t* lhs, ListNode_t* rhs)
{
    return IsEqual(strcmp(List_Node(lhs, TaskNode_t, head.lHead)->task.name, List_Node(rhs, TaskNode_t, head.lHead)->task.name), 0);
}

static TaskNode_t* FindTaskByName(const char* name)
{
    TaskNode_t* ret = nullptr;

    if (!IsEqual(strcmp(name, pIdleTaskNode->task.name), 0))
    {
        TaskNode_t target;

        target.task.name = strdup(name);

        ret = List_Node(List_FindNode(pTaskPool, StructOffset(&target, TaskNode_t, head.lHead), FindTarget), TaskNode_t, head.lHead);

        free(target.task.name);
    }

    return ret;
}

static bool WaitTask(const char* name)
{
    TaskNode_t* taskNode = FindTaskByName(name);

    bool ret = (!IsEqual(taskNode, nullptr));

    if (ret)
    {
        RunningToWait(taskNode->task.wait);

        Schedule();
    }

    return ret;
}

static bool SleepTask(uint32_t ms)
{
    bool ret = !IsEqual(gCurrentTaskAddr, StructOffset(pIdleTaskNode, TaskNode_t, task));

    if (ret)
    {
        RunningToSleep(ms);

        Schedule();
    }

    return ret;
}

uint32_t TaskCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2)
{
    uint32_t ret = -1;

    switch (cmd)
    {
        case SysCall_Task_Kill: {
            KillTask();
            break;
        }
        case SysCall_Task_Schedule: {
            SleepToReady();
            RunningToReady();
            Schedule();
            break;
        }
        case SysCall_Task_Register: {
            ret = IsEqual(CreateTaskToReady((void*)(param1)), false) ? 0 : 1;
            break;
        }
        case SysCall_Task_Wait: {
            WaitTask((void*)(param1));
            break;
        }
        case SysCall_Task_Sleep: {
            SleepTask(param1);
            break;
        }
        default:
            break;
    }

    return ret;
}