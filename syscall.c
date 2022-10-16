#include "syscall.h"
#include "interrupt.h"
#include "memory.h"
#include "screen.h"
#include "assert.h"
#include "task.h"

extern void SysCallInit();

void SystemCallModuleInit()
{
	SetInterruptGate(0x80, (u32)(SysCallHanderEntry));
}

void Exit(int status)
{
	SysCall(SysCall_Task, SysCall_Task_Kill, status, (u32)(nullptr));
}

void Debug()
{
	asm volatile("int $0x03");
}	

bool RegisterApp(const char* name, pFunc tMain, u8 priority)
{
	bool ret = false;

	AppInfo* appInfo = malloc(sizeof(AppInfo));

	appInfo->name = name;

	appInfo->tMain = tMain;

	appInfo->priority = priority;

	ret = IsEqual(SysCall(SysCall_Task, SysCall_Task_Register, (u32)(appInfo), (u32)(nullptr)), 0) ? false : true;

	free(appInfo);

	return ret;
}

void Wait(const char* name)
{
	SysCall(SysCall_Task, SysCall_Task_Wait, (u32)(name), (u32)(nullptr));
}

void Sleep(u32 ms)
{
	SysCall(SysCall_Task, SysCall_Task_Sleep, ms, (u32)(nullptr));
}

u16 printf(const char* format, ...)
{
	u16 ret = 0;

	va_list v_arg;

	va_start(v_arg, format);

	ret = SysCall(SysCall_Screen, SysCall_Screen_Printf, (u32)(format), (u32)(v_arg));

	va_end(v_arg);

	return ret;
}