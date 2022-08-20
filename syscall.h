#ifndef SYSCALL_H
#define SYSCALL_H

#include <utility.h>

#ifndef SysCall
#define SysCall(type, cmd, param1, param2)  \
	asm volatile(                           \
		"movl $" #type ", %%eax\n"          \
		"movl $" #cmd  ", %%ebx\n"          \
		"movl %0,         %%ecx\n"          \
		"movl %1,         %%edx\n"          \
		"int              $0x80\n"          \
		:                                   \
		: "r"(param1), "r"(param2)          \
		: "eax", "ebx", "ecx", "edx"        \
	)
#endif	

void SystemCallModuleInit();

void Exit();
void Debug();
void Wait(const char* name);
void RegisterApp(const char* name, pFunc tMain, u8 priority);

#endif //!SYSCALL_H