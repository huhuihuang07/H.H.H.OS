#pragma once

#include "utility.h"
#include "mutex.h"

#define SYS_CALL_NUMBER 0x80

typedef enum {
    SysCall_Task   = 0,
    SysCall_Screen = 1,
    SysCall_Mutex  = 2
} SysCall_TYPE_t;

void SystemCallModuleInit();

void Exit(int32_t status);
void Debug();
void Wait(const char* name);
void Sleep(uint32_t ms);
bool RegisterApp(const char* name, pFunc_t tMain, uint8_t priority);
uint32_t printf(const char* format, ...);

uint32_t CreateMutex(Mutex_type_t type);
void EnterCritical(uint32_t ptr);
void ExitCritical(uint32_t ptr);
bool DestroyMutex(uint32_t ptr);

extern void SysCallHandlerEntry();
extern uint32_t SysCall(uint32_t type, uint32_t cmd, uint32_t param1, uint32_t param2);