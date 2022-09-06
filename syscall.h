#ifndef SYSCALL_H
#define SYSCALL_H

#include "utility.h"

typedef enum {
	SysCall_Task = 0,
}SysCall_TYPE;

void SystemCallModuleInit();

void Exit(int status);
void Debug();
void Wait(const char* name);
bool RegisterApp(const char* name, pFunc tMain, u8 priority);

extern u32 SysCall(u32 type, u32 cmd, u32 param1, u32 param2);  

#endif //!SYSCALL_H