#ifndef SYSCALL_H
#define SYSCALL_H

#include <utility.h>

#ifndef SysCall
#define SysCall(type, cmd, param1, param2, ret)  \
	asm volatile(                                \
		"movl $" #type ", %%eax\n"               \
		"movl $" #cmd  ", %%ebx\n"               \
		"movl %1,         %%ecx\n"               \
		"movl %2,         %%edx\n"               \
		"int              $0x80\n"               \
		"movl %%eax,         %0\n"               \
		: "=r"(ret)                              \
		: "r"(param1), "r"(param2)               \
		: "eax", "ebx", "ecx", "edx"             \
	)
#endif	

void SystemCallModuleInit();

void Exit(int status);
void Debug();
void Wait(const char* name);
bool RegisterApp(const char* name, pFunc tMain, u8 priority);

#endif //!SYSCALL_H