#pragma once

#include "type.h"
#include "list.h"

typedef enum {
    SysCall_Mutex_Create  = 0,
    SysCall_Mutex_Enter   = 1,
    SysCall_Mutex_Exit    = 2,
    SysCall_Mutex_Destroy = 3
} SysCall_Mutex_CMD_t;

uint32_t MutexCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2);

uint32_t Sys_CreateMutex();
void Sys_EnterCritical(uint32_t mutex);
void Sys_ExitCritical(uint32_t mutex);
void Sys_DestroyMutex(uint32_t mutex);