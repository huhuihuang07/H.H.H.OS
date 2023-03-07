#pragma once

#include "type.h"

int32_t Kmain(int32_t argc, char* argv[]);

extern void SystemCallModuleInit();

extern void TaskModuleInit();

extern void LaunchTask();