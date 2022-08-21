#include <syscall.h>
#include <memory.h>
#include <task.h>

extern void SysCallInit();

void SystemCallModuleInit()
{
	SysCallInit();
}

void Exit(int status)
{
	u32 ret = 0;

	SysCall(0, 0, status, nullptr, ret);
}

void Debug()
{
	asm volatile("int $0x03");
}	

bool RegisterApp(const char* name, pFunc tMain, u8 priority)
{
	u32 ret = -1;

	AppInfo* appInfo = malloc(sizeof(AppInfo));

	appInfo->name = name;

	appInfo->tMain = tMain;

	appInfo->priority = priority;

	SysCall(0, 3, appInfo, nullptr, ret);

	free(appInfo);

	return IsEqual(ret, 0) ? false : true;
}

void Wait(const char* name)
{
	u32 ret = -1;

	SysCall(0, 4, name, nullptr, ret);
}