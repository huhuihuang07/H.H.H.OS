#include "task.h"
#include "string.h"
#include "memory.h"
#include "syscall.h"
#include "kernel.h"
#include "screen.h"

#define MAX_RUNNING_TASK 4u
#define MAX_RUNNING_TICK (255u + 5u)

volatile Task_t* gCurrentTaskAddr = nullptr;

static TSS_t* pTSS = nullptr;

static Queue_t* pReadyQueue = nullptr;

static Queue_t* pRunningQueue = nullptr;

static List_t* pSleepList = nullptr;

static List_t* pTaskPool = nullptr;

static TaskNode_t* pIdleTaskNode = nullptr;

static int IdleTask(int argc, const char* argv[], const char* env[])
{
    while (true)
        ;
}

static void TaskEntry()
{
    int ret = 0u;

    if (!IsEqual(gCurrentTaskAddr, nullptr))
    {
        ret = gCurrentTaskAddr->tMain(0, nullptr, nullptr);
    }

    Exit(ret);
}

static void InitTSS()
{
    pTSS = malloc(sizeof(TSS_t));

    SetDescValue(AddrOffset(gGdtInfo.entry, GDT_TSSIndex), (uint32_t)(pTSS), sizeof(TSS_t) - 1u, DA_386TSS + DA_DPL0);

    pTSS->ss0    = GDT_FlatModeDataSelector;
    pTSS->esp0   = BaseOfLoader;
    pTSS->io_map = sizeof(TSS_t);

    uint16_t ax = GDT_TSSSelector;

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

    SetDescValue(AddrOffset(gGdtInfo.entry, GDT_LDTIndex), (uint32_t)(StructOffset(pTask, Task_t, ldt)), sizeof(pTask->ldt) - 1u, DA_LDT + DA_DPL0);

    asm volatile(
        "movw %0, %%ax\n"
        "lldt     %%ax\n"
        :
        : "r"(pTask->LDTSelector)
        : "ax");
}

static void InitTaskDataStruct()
{
    pReadyQueue = malloc(sizeof(Queue_t));

    Queue_Init(pReadyQueue);

    pRunningQueue = malloc(sizeof(Queue_t));

    Queue_Init(pRunningQueue);

    pSleepList = malloc(sizeof(List_t));

    List_Init(pSleepList);

    pTaskPool = malloc(sizeof(List_t));

    List_Init(pTaskPool);
}

static TaskNode_t* AppInfoToTaskNode(const AppInfo* appInfo)
{
    TaskNode_t* ret = malloc(sizeof(TaskNode_t));

    ret->task.stack = PageAlloc(nullptr);

    SetDescValue(AddrOffset(ret->task.ldt, LDT_Code32Index), 0u, 0xfffff, DA_32 + DA_C + DA_LIMIT_4K + DA_DPL3);
    SetDescValue(AddrOffset(ret->task.ldt, LDT_Data32Index), 0u, 0xfffff, DA_32 + DA_DRW + DA_LIMIT_4K + DA_DPL3);
    SetDescValue(AddrOffset(ret->task.ldt, LDT_Stack32Index), 0u, PGEIndex((uint32_t)(ret->task.stack)) - 1u, DA_32 + DA_SRW + DA_LIMIT_4K + DA_DPL3);

    ret->task.rv.gs = GDT_UndefinedSelector;

    ret->task.rv.fs = LDT_Data32Selector;
    ret->task.rv.es = LDT_Data32Selector;
    ret->task.rv.ds = LDT_Data32Selector;

    ret->task.rv.ss = LDT_Stack32Selector;

    ret->task.rv.esp = (uint32_t)(ret->task.stack) + PAGE_SIZE;

    ret->task.rv.cs  = LDT_Code32Selector;
    ret->task.rv.eip = (uint32_t)(TaskEntry);

    ret->task.rv.eflags = 0x3202;

    ret->task.LDTSelector = GDT_LDTSelector;

    ret->task.wait = malloc(sizeof(Queue_t));

    Queue_Init(ret->task.wait);

    ret->task.tMain = appInfo->tMain;

    ret->task.total = MAX_RUNNING_TICK - appInfo->priority;

    ret->task.name = IsEqual(appInfo->name, nullptr) ? nullptr : strdup(appInfo->name);

    return ret;
}

static bool CreateTaskToReady(const AppInfo* appInfo)
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
    AppInfo appInfo = {"IdleTask", IdleTask, 255u};

    pIdleTaskNode = AppInfoToTaskNode(&appInfo);
}

static void InitInitTask()
{
    AppInfo appInfo = {"InitInitTask", AMain, 255u};

    CreateTaskToReady(&appInfo);
}

void TaskModuleInit()
{
    InitTSS();

    InitTaskDataStruct();

    InitIdleTask();

    InitInitTask();
}

static bool RunningQueueIsFull()
{
    return (Queue_Length(pRunningQueue) > MAX_RUNNING_TASK);
}

static bool TaskIsIdle()
{
    return IsEqual(gCurrentTaskAddr, StructOffset(pIdleTaskNode, TaskNode_t, task)) ? true : false;
}

static void ReadyToRunning()
{
    while ((!RunningQueueIsFull()) && (!Queue_IsEmpty(pReadyQueue)))
    {
        TaskNode_t* taskNode = List_Node(Queue_Remove(pReadyQueue), TaskNode_t, head.qHead);

        taskNode->task.current = 0u;

        Queue_Add(pRunningQueue, StructOffset(taskNode, TaskNode_t, head.qHead));
    }
}

static void RunningToReady()
{
    if (!TaskIsIdle())
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
        ListNode_t* pCurListNode = pSleepList->next;
        ListNode_t* pNexListNode = nullptr;

        TaskNode_t* pTaskNode = nullptr;

        while (!IsEqual(pCurListNode, pSleepList))
        {
            pTaskNode = List_Node(pCurListNode, TaskNode_t, head.qHead);

            if (IsEqual(--pTaskNode->task.ticks, 0u))
            {
                pNexListNode = pCurListNode->next;

                List_DelNode(pCurListNode);

                Queue_Add(pReadyQueue, StructOffset(pTaskNode, TaskNode_t, head.qHead));

                pCurListNode = pNexListNode;
            }
            else
            {
                pCurListNode = pCurListNode->next;
            }
        }
    }
}

void LaunchTask()
{
    ReadyToRunning();

    gCurrentTaskAddr = Queue_IsEmpty(pRunningQueue) ? StructOffset(pIdleTaskNode, TaskNode_t, task) : StructOffset(List_Node(Queue_Front(pRunningQueue), TaskNode_t, head.qHead), TaskNode_t, task);

    PrepareForRun(gCurrentTaskAddr);

    ClockInit();

    RunTask(gCurrentTaskAddr);
}

void ScheduleNext()
{
    ReadyToRunning();

    Task_t* pNextTask = Queue_IsEmpty(pRunningQueue) ? StructOffset(pIdleTaskNode, TaskNode_t, task) : (Queue_Rotate(pRunningQueue), StructOffset(List_Node(Queue_Front(pRunningQueue), TaskNode_t, head.qHead), TaskNode_t, task));

    if (!IsEqual(pNextTask, gCurrentTaskAddr))
    {
        PrepareForRun(gCurrentTaskAddr = pNextTask);
    }
}

static void Schedule()
{
    SleepToReady();

    RunningToReady();

    ScheduleNext();
}

void KillTask()
{
    if (!TaskIsIdle() && !Queue_IsEmpty(pRunningQueue))
    {
        TaskNode_t* pCurrentTask = List_Node(Queue_Remove(pRunningQueue), TaskNode_t, head.qHead);

        WaitToReady(pCurrentTask->task.wait);

        List_DelNode(StructOffset(pCurrentTask, TaskNode_t, head.lHead));

        PageFree(pCurrentTask->task.stack);

        free(pCurrentTask->task.wait);

        free(pCurrentTask->task.name);

        free(pCurrentTask);

        ScheduleNext();
    }
}

void WaitToReady(Queue_t* pWaitQueue)
{
    while (!Queue_IsEmpty(pWaitQueue))
    {
        Queue_Add(pReadyQueue, Queue_Remove(pWaitQueue));
    }
}

void RunningToWait(Queue_t* pWaitQueue)
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

    gCurrentTaskAddr->ticks = ticks > 0u ? ticks : 1u;

    List_Add(pSleepList, Queue_Remove(pRunningQueue));
}

static bool FindTarget(ListNode_t* lhs, ListNode_t* rhs)
{
    return IsEqual(strcmp(List_Node(lhs, TaskNode_t, head.lHead)->task.name, List_Node(rhs, TaskNode_t, head.lHead)->task.name), 0u);
}

static TaskNode_t* FindTaskByName(const char* name)
{
    TaskNode_t* ret = nullptr;

    if (!IsEqual(strcmp(name, pIdleTaskNode->task.name), 0u))
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

        ScheduleNext();
    }

    return ret;
}

static bool SleepTask(uint32_t ms)
{
    bool ret = !TaskIsIdle();

    if (ret)
    {
        RunningToSleep(ms);

        ScheduleNext();
    }

    return ret;
}

uint32_t GetCurrentTaskID()
{
    return (uint32_t)(gCurrentTaskAddr);
}

uint32_t TaskCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2)
{
    uint32_t ret = 0u;

    switch (cmd)
    {
        case SysCall_Task_Kill: {
            KillTask();
            break;
        }
        case SysCall_Task_Schedule: {
            Schedule();
            break;
        }
        case SysCall_Task_Register: {
            ret = CreateTaskToReady((void*)(param1)) ? 1u : 0u;
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