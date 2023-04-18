#include "mutex.h"
#include "memory.h"
#include "task.h"
#include "pic.h"

uint32_t Sys_CreateMutex()
{
    Mutex_t* mutex = (Mutex_t*)(malloc(sizeof(Mutex_t)));

    mutex->lock = false;

    mutex->queue = (Queue_t*)(malloc(sizeof(Queue_t)));

    Queue_Init(mutex->queue);

    return (uint32_t)(mutex);
}

bool Sys_EnterCritical(uint32_t mutex)
{
    Mutex_t* pMutex = (Mutex_t*)(mutex);

    state_t state = SetIFState(Disable);

    if (pMutex->lock)
    {
        gCurrentTaskAddr->rv.eax = 0u;

        RunningToWait(pMutex->queue);

        Schedule();
    }
    else
    {
        pMutex->lock = true;
    }

    SetIFState(state);

    return true;
}

void Sys_ExitCritical(uint32_t mutex)
{
    Mutex_t* pMutex = (Mutex_t*)(mutex);

    state_t state = SetIFState(Disable);

    if (pMutex->lock)
    {
        pMutex->lock = false;

        WaitToReady(pMutex->queue);
    }

    SetIFState(state);
}

void Sys_DestroyMutex(uint32_t mutex)
{
    Mutex_t* pMutex = (Mutex_t*)(mutex);

    state_t state = SetIFState(Disable);

    WaitToReady(pMutex->queue);

    free(pMutex->queue);

    free(pMutex);

    SetIFState(state);
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
            ret = Sys_EnterCritical(param1);
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