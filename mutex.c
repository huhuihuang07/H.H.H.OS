#include "mutex.h"
#include "screen.h"

uint32_t Sys_CreateMutex()
{
    void* mutex = nullptr;

    print("%s ptr = %x\n", __FUNCTION__, mutex);

    return (uint32_t)(mutex);
}

void Sys_EnterCritical(uint32_t mutex)
{
    print("%s ptr = %x\n", __FUNCTION__, mutex);
}

void Sys_ExitCritical(uint32_t mutex)
{
    print("%s ptr = %x\n", __FUNCTION__, mutex);
}

void Sys_DestroyMutex(uint32_t mutex)
{
    print("%s ptr = %x\n", __FUNCTION__, mutex);
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
            Sys_EnterCritical(param1);
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