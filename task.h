#pragma once

#include "kernel.h"
#include "queue.h"

#define TASK_LDT_LEN 3u

typedef struct
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t kesp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} RegisterValue_t;

typedef struct
{
    uint32_t previous;    // 4
    uint32_t esp0;        // 4
    uint32_t ss0;         // 4
    uint32_t unused[22u]; // 4 * 22 = 88
    uint16_t reserved;    // 2
    uint16_t iomb;        // 2
} TSS_t;                  /* Task_t state segment */

typedef struct
{
    RegisterValue_t rv;             // sizeof(uint32_t) * 18 = 4 * 18 = 72
    Descriptor_t ldt[TASK_LDT_LEN]; // sizeof(Descriptor_t) * TASK_LDT_LEN = 8 * 3 = 24
    pid_t pid;
    pid_t ppid;
    pFunc_t tMain;
    Queue_t* wait;
    int8_t* stack;
    char* name;
    uint32_t ticks;
    uint16_t ldtSelector;
    uint16_t total;
    uint16_t current;
} Task_t;

typedef struct
{
    ListNode_t lHead;
    QueueNode_t qHead;
} Head_t;

typedef struct
{
    Head_t head;
    Task_t task;
} TaskNode_t;

typedef struct
{
    const char* name;
    pFunc_t tMain;
    uint8_t priority;
} AppInfo;

typedef enum {
    SysCall_Task_Kill     = 0,
    SysCall_Task_Schedule = 1,
    SysCall_Task_Register = 2,
    SysCall_Task_Wait     = 3,
    SysCall_Task_Sleep    = 4,
} SysCall_TASK_CMD_t;

void ScheduleNext();

void KillTask();

void WaitToReady(Queue_t* pWaitQueue);

void RunningToWait(Queue_t* pWaitQueue);

uint32_t TaskCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2);

void TaskModuleInit();

void LaunchTask();

extern void RunTask(volatile const Task_t* const pTask);

extern void ClockInit();

extern int AMain(int argc, const char* argv[], const char* env[]);

extern uint32_t jiffy;

volatile Task_t* gCurrentTaskAddr;