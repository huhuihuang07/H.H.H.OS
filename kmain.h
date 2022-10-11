#ifndef KMAIN_H
#define KMAIN_H

#include "type.h"

int Kmain(int argc, char* argv[]);

extern u16 print(const char* format, ...);

extern void InterruptModuleInit();

extern void SystemCallModuleInit();

extern void TaskModuleInit();

extern void LaunchTask();

#endif // !KMAIN_H