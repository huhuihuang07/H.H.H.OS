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
} Mutex_type;

typedef struct
{
    ListNode_t* head;
    Queue_t* queue;
    Mutex_type type;
    uint32_t lock;
} Mutex_t;

static List_t* gMutexList;

void MutexModuleInit();

uint32_t MutexCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2);

static uint32_t Sys_CreateMutex(Mutex_type type);
static bool Sys_EnterCritical(uint32_t mutex);
static void Sys_ExitCritical(uint32_t mutex);
static bool Sys_DestroyMutex(uint32_t mutex);
static bool IsMutexValid(uint32_t mutex);