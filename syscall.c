#include <syscall.h>
#include <memory.h>
#include <task.h>

extern void SysCallInit();

void SystemCallModuleInit()
{
	SysCallInit();
}

void Exit()
{
	SysCall(0, 0, nullptr, nullptr);
}

void Debug()
{
	asm volatile("int $0x03");
}	

void RegisterApp(const char* name, pFunc tMain, u8 priority)
{
	AppInfo* appInfo = malloc(sizeof(AppInfo));

	appInfo->name = name;

	appInfo->tMain = tMain;

	appInfo->priority = priority;

	SysCall(0, 3, appInfo, nullptr);

	free(appInfo);
}

void Wait(const char* name)
{
	SysCall(0, 4, name, nullptr);
}