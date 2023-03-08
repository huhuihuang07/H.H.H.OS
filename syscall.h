#pragma once

#include "utility.h"

typedef enum {
    SysCall_Task = 0,
    SysCall_Screen = 1,
} SysCall_TYPE_t;

void SystemCallModuleInit();

void Exit(int32_t status);
void Debug();
void Wait(const char* name);
void Sleep(uint32_t ms);
bool RegisterApp(const char* name, pFunc_t tMain, uint8_t priority);
uint16_t printf(const char* format, ...);

extern void SysCallHanderEntry();
extern uint32_t SysCall(uint32_t type, uint32_t cmd, uint32_t param1, uint32_t param2);