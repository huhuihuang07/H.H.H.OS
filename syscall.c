#include <syscall.h>
#include <screen.h>

extern void SysCallInit();
extern void KillTask();

#define MAX_SYSCALL_NUMBER 256

static pFunc gSysCallFunc[MAX_SYSCALL_NUMBER] = {nullptr};

void SystemCallModuleInit()
{
	SysCallInit();

	gSysCallFunc[0] = KillTask;
}

void SystemCall(u32 param)
{
	if((0 <= param) && (param < MAX_SYSCALL_NUMBER))
	{
		if(!IsEqual(gSysCallFunc[param], nullptr))
		{
			gSysCallFunc[param]();
		}
	}
}