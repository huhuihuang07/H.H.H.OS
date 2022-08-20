#ifndef SYSCALL_H
#define SYSCALL_H

#include <utility.h>

#ifndef SysCall
#define SysCall(type, cmd, param1, param2)  \
	asm volatile(                           \
		"movl $" #type ", %%eax\n"          \
		"movl $" #cmd  ", %%ebx\n"          \
		"int              $0x80\n"          \
		:                                   \
		: "ecx"(param1), "edx"(param2)      \
		: "eax", "ebx", "ecx", "edx"        \
	)
#endif	

void SystemCallModuleInit();

void Exit();
void Debug();

#endif //!SYSCALL_H