#include <syscall.h>

extern void SysCallInit();

void SystemCallModuleInit()
{
	SysCallInit();
}

void Exit()
{
	SysCall(0, 0, 0, 0);
}

void Debug()
{
	asm volatile("int $0x03");
}	