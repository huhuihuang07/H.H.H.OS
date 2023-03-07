#include "syscall.h"
#include "interrupt.h"
#include "memory.h"
#include "screen.h"
#include "assert.h"
#include "task.h"

extern void SysCallInit();

void SystemCallModuleInit()
{
    SetInterruptGate(0x80, (uint32_t)(SysCallHanderEntry));
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

    ret = IsEqual(SysCall(SysCall_Task, SysCall_Task_Register, (uint32_t)(appInfo), (uint32_t)(nullptr)), 0) ? false : true;

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

uint16_t printf(const char* format, ...)
{
    uint16_t ret = 0;

    va_list v_arg;

    va_start(v_arg, format);

    ret = SysCall(SysCall_Screen, SysCall_Screen_Printf, (uint32_t)(format), (uint32_t)(v_arg));

    va_end(v_arg);

    return ret;
}