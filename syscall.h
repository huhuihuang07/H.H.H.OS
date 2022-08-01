#ifndef SYSCALL_H
#define SYSCALL_H

#include <utility.h>

#ifndef SysCall
#define SysCall(param)     \
	asm volatile(          \
		"movl %0, %%eax\n" \
		"int      $0x80\n" \
		:                  \
		: "r"(param)       \
		: "ax"             \
	)
#endif	

void SystemCall(u32 param);
void SystemCallModuleInit();

#endif //!SYSCALL_H