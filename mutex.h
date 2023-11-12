#pragma once

#include "type.h"
#include "queue.h"

typedef enum {
    SysCall_Mutex_Create  = 0,
    SysCall_Mutex_Enter   = 1,
    SysCall_Mutex_Exit    = 2,
    SysCall_Mutex_Destroy = 3
} SysCall_Mutex_CMD_t;

typedef enum {
    Normal,
    Strict
} Mutex_type_t;

typedef struct
{
    Queue_t* queue;
    Mutex_type_t type;
    uint32_t lock;
} Mutex_t;

void MutexModuleInit();

uint32_t MutexCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2);