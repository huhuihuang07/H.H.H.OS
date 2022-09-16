#include "syscall.h"
#include "memory.h"
#include "task.h"

extern void SysCallInit();

void SystemCallModuleInit()
{
	SysCallInit();
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