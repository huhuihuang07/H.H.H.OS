#include "mutex.h"
#include "memory.h"
#include "task.h"
#include "pic.h"

static List_t* gMutexList = nullptr;

void MutexModuleInit()
{
    List_t* gMutexList = (List_t*)(malloc(sizeof(List_t)));

    List_Init(gMutexList);
}

static uint32_t Sys_CreateMutex()
{
    Mutex_t* mutex = (Mutex_t*)(malloc(sizeof(Mutex_t)));

    mutex->lock = false;

    mutex->queue = (Queue_t*)(malloc(sizeof(Queue_t)));

    Queue_Init(mutex->queue);

    List_Add(gMutexList, StructOffset(mutex, Mutex_t, head));

    return (uint32_t)(mutex);
}

static bool Sys_EnterCritical(uint32_t mutex)
{
    if (IsMutexValid(mutex))
    {
        state_t state = SetIFState(Disable);

        Mutex_t* pMutex = (Mutex_t*)(mutex);

        if (IsEqual(pMutex->lock, true))
        {
            gCurrentTaskAddr->rv.eax = 0u;

            RunningToWaitting(pMutex->queue);

            Schedule();

            RunTask(gCurrentTaskAddr);
        }
        else
        {
            pMutex->lock = true;
        }

        SetIFState(state);
    }

    return true;
}

static void Sys_ExitCritical(uint32_t mutex)
{
    if (IsMutexValid(mutex))
    {
        state_t state = SetIFState(Disable);

        Mutex_t* pMutex = (Mutex_t*)(mutex);

        if (IsEqual(pMutex->lock, true))
        {
            pMutex->lock = false;

            WaittingToReady(pMutex->queue);
        }

        SetIFState(state);
    }
}

static void Sys_DestroyMutex(uint32_t mutex)
{
    if (IsMutexValid(mutex))
    {
        state_t state = SetIFState(Disable);

        Mutex_t* pMutex = (Mutex_t*)(mutex);

        WaittingToReady(pMutex->queue);

        List_DelNode(StructOffset(pMutex, Mutex_t, head));

        free(pMutex->queue);

        free(pMutex);

        SetIFState(state);
    }
}

static bool IsMutexValid(uint32_t mutex)
{
    bool ret = false;

    ListNode_t* pos = nullptr;

    List_ForEach(gMutexList, pos)
    {
        if (ret = IsEqual(pos, mutex))
        {
            break;
        }
    }

    return ret;
}

uint32_t MutexCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2)
{
    uint32_t ret = 0u;

    switch (cmd)
    {
        case SysCall_Mutex_Create: {
            ret = Sys_CreateMutex();
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
            Sys_DestroyMutex(param1);
            break;
        }
        default:
            break;
    }

    return ret;
}