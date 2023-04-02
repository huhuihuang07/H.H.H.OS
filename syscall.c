#include "syscall.h"
#include "interrupt.h"
#include "mutex.h"
#include "memory.h"
#include "screen.h"
#include "task.h"

void SystemCallModuleInit()
{
    SetInterruptGate(SYS_CALL_NUMBER, (uint32_t)(SysCallHanderEntry));
}

void Exit(int32_t status)
{
    SysCall(SysCall_Task, SysCall_Task_Kill, status, (uint32_t)(nullptr));
}

void Debug()
{
    asm volatile("int $0x03");
}

bool RegisterApp(const char* name, pFunc_t tMain, uint8_t priority)
{
    bool ret = false;

    AppInfo* appInfo = malloc(sizeof(AppInfo));

    appInfo->name = name;

    appInfo->tMain = tMain;

    appInfo->priority = priority;

    ret = IsEqual(SysCall(SysCall_Task, SysCall_Task_Register, (uint32_t)(appInfo), (uint32_t)(nullptr)), 0u) ? false : true;

    free(appInfo);

    return ret;
}

void Wait(const char* name)
{
    SysCall(SysCall_Task, SysCall_Task_Wait, (uint32_t)(name), (uint32_t)(nullptr));
}

void Sleep(uint32_t ms)
{
    SysCall(SysCall_Task, SysCall_Task_Sleep, ms, (uint32_t)(nullptr));
}

uint32_t printf(const char* format, ...)
{
    uint32_t ret = 0u;

    va_list v_arg;

    va_start(v_arg, format);

    ret = SysCall(SysCall_Screen, SysCall_Screen_Printf, (uint32_t)(format), (uint32_t)(v_arg));

    va_end(v_arg);

    return ret;
}

uint32_t CreateMutex()
{
    return SysCall(SysCall_Mutex, SysCall_Mutex_Create, (uint32_t)(nullptr), (uint32_t)(nullptr));
}

void EnterCritical(uint32_t ptr)
{
    SysCall(SysCall_Mutex, SysCall_Mutex_Enter, ptr, (uint32_t)(nullptr));
}

void ExitCritical(uint32_t ptr)
{
    SysCall(SysCall_Mutex, SysCall_Mutex_Exit, ptr, (uint32_t)(nullptr));
}

void DestroyMutex(uint32_t ptr)
{
    SysCall(SysCall_Mutex, SysCall_Mutex_Destroy, ptr, (uint32_t)(nullptr));
}