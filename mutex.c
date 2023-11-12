#include "mutex.h"
#include "memory.h"
#include "task.h"
#include "pic.h"

static List_t* gMutexList = nullptr;

typedef struct
{
    ListNode_t head;
    Mutex_t* mutex;
} MutexNode_t;

void MutexModuleInit()
{
    List_t* gMutexList = (List_t*)(malloc(sizeof(List_t)));

    List_Init(gMutexList);
}

static uint32_t Sys_CreateMutex(Mutex_type_t type)
{
    Mutex_t* mutex = (Mutex_t*)(malloc(sizeof(Mutex_t)));

    mutex->queue = (Queue_t*)(malloc(sizeof(Queue_t)));

    Queue_Init(mutex->queue);

    mutex->lock = 0u;

    mutex->type = type;

    MutexNode_t* node = (MutexNode_t*)(malloc(sizeof(MutexNode_t)));

    node->mutex = mutex;

    List_Add(gMutexList, StructOffset(node, MutexNode_t, head));

    return (uint32_t)(mutex);
}

static bool FindTarget(ListNode_t* lhs, ListNode_t* rhs)
{
    return IsEqual(List_Node(lhs, MutexNode_t, head)->mutex, List_Node(rhs, MutexNode_t, head)->mutex);
}

static MutexNode_t* GetMutexNodeByList(uint32_t mutex)
{
    MutexNode_t* ret = nullptr;

    MutexNode_t node = {};

    node.mutex = (Mutex_t*)(mutex);

    ListNode_t* target = List_FindNode(gMutexList, StructOffset(&node, MutexNode_t, head), FindTarget);

    if (!IsEqual(target, nullptr))
    {
        ret = List_Node(target, MutexNode_t, head);
    }

    return ret;
}

static bool IsMutexValid(uint32_t mutex)
{
    return IsEqual(GetMutexNodeByList(mutex), nullptr) ? false : true;
}

static void MutexSchedule(Mutex_t* pMutex)
{
    gCurrentTaskAddr->rv.eax = 0u;

    RunningToWait(pMutex->queue);

    ScheduleNext();

    RunTask(gCurrentTaskAddr);
}

static bool MutexNormalEnter(Mutex_t* pMutex)
{
    if (IsEqual(pMutex->lock, 0u))
    {
        pMutex->lock = (uint32_t)(gCurrentTaskAddr);
    }
    else
    {
        MutexSchedule(pMutex);
    }

    return true;
}

static bool MutexStrictEnter(Mutex_t* pMutex)
{
    if (IsEqual(pMutex->lock, gCurrentTaskAddr) || IsEqual(pMutex->lock, 0u))
    {
        pMutex->lock = (uint32_t)(gCurrentTaskAddr);
    }
    else
    {
        MutexSchedule(pMutex);
    }

    return true;
}

static bool Sys_EnterCritical(uint32_t mutex)
{
    if (IsMutexValid(mutex))
    {
        state_t state = SetIFState(Disable);

        Mutex_t* pMutex = (Mutex_t*)(mutex);

        Mutex_type_t type = pMutex->type;

        switch (type)
        {
            case Normal: MutexNormalEnter(pMutex); break;
            case Strict: MutexStrictEnter(pMutex); break;
        }

        SetIFState(state);
    }

    return true;
}

static void MutexNormalExit(Mutex_t* pMutex)
{
    if (!IsEqual(pMutex->lock, 0u))
    {
        pMutex->lock = 0u;

        WaitToReady(pMutex->queue);
    }
}

static void MutexStrictExit(Mutex_t* pMutex)
{
    if (IsEqual(pMutex->lock, gCurrentTaskAddr))
    {
        pMutex->lock = 0u;

        WaitToReady(pMutex->queue);
    }
    else
    {
        KillTask();

        RunTask(gCurrentTaskAddr);
    }
}

static void Sys_ExitCritical(uint32_t mutex)
{
    if (IsMutexValid(mutex))
    {
        state_t state = SetIFState(Disable);

        Mutex_t* pMutex = (Mutex_t*)(mutex);

        Mutex_type_t type = pMutex->type;

        switch (type)
        {
            case Normal: MutexNormalExit(pMutex); break;
            case Strict: MutexStrictExit(pMutex); break;
        }

        SetIFState(state);
    }
}

static bool Sys_DestroyMutex(uint32_t mutex)
{
    Mutex_t* pMutex = (Mutex_t*)(mutex);

    bool ret = IsMutexValid(mutex) && (IsEqual(pMutex->lock, 0u)) && (Queue_IsEmpty(pMutex->queue));

    if (ret)
    {
        state_t state = SetIFState(Disable);

        free(pMutex->queue);

        free(pMutex);

        MutexNode_t* node = GetMutexNodeByList(mutex);

        List_DelNode(StructOffset(node, MutexNode_t, head));

        free(node);

        SetIFState(state);
    }

    return ret;
}

uint32_t MutexCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2)
{
    uint32_t ret = 0u;

    switch (cmd)
    {
        case SysCall_Mutex_Create: {
            ret = Sys_CreateMutex(param1);
            break;
        }
        case SysCall_Mutex_Enter: {
            ret = Sys_EnterCritical(param1) ? 1u : 0u;
            break;
        }
        case SysCall_Mutex_Exit: {
            Sys_ExitCritical(param1);
            break;
        }
        case SysCall_Mutex_Destroy: {
            ret = Sys_DestroyMutex(param1) ? 1u : 0u;
            break;
        }
        default:
            break;
    }

    return ret;
}