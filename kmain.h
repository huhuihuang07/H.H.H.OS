#pragma once

#include "type.h"

void Kernel_init();

int32_t Kmain(int32_t argc, char* argv[]);

extern void ScreenInit();
extern void MemoryModuleInit();
extern void InterruptModuleInit();
extern void SystemCallModuleInit();
extern void MutexModuleInit();
extern void KeyBoardModuleInit();
extern void TaskModuleInit();
extern void LaunchTask();