#ifndef TASK_H
#define TASK_H

#include "kernel.h"
#include "queue.h"

#define TASK_LDT_LEN  3

typedef struct
{
	u32 gs;
	u32 fs;
	u32 es;
	u32 ds;
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 kesp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u32 error_code;
	u32 eip;      
	u32 cs;
	u32 eflags;
	u32 esp;
	u32 ss;
}RegisterValue;

typedef struct
{
	u32 previous;    // 4
	u32 esp0;        // 4
	u32 ss0;         // 4
	u32 unused[22];  // 4 * 22 = 88
	u16 reserved;    // 2
	u16 iomb;        // 2
}TSS; /*Task state segment*/

typedef struct
{
	RegisterValue rv;             // sizeof(u32) * 18 = 4 * 18 = 72
	Descriptor ldt[TASK_LDT_LEN]; // sizeof(Descriptor) * TASK_LDT_LEN = 8 * 3 = 24
	pid_t pid;              
	pid_t ppid;      
	pFunc tMain;                  
	Queue* wait;                  
	int8* stack;                  
	char* name;        
	u32 ticks;           
	u16 ldtSelector;              
	u16 total;					  
	u16 current;               
}Task;

typedef struct
{
	ListNode  lHead;
	QueueNode qHead;
}Head;

typedef struct
{
	Head head;
	Task task;
}TaskNode;

typedef struct{
	const char* name;
	pFunc tMain;
	u8 priority;
}AppInfo;

typedef enum{
	SysCall_Task_Kill     = 0,
	SysCall_Task_Schedule = 1,
	SysCall_Task_Register = 2,
	SysCall_Task_Wait     = 3,
	SysCall_Task_Sleep    = 4,
}SysCall_TASK_CMD;

static TaskNode* AppInfoToTaskNode(AppInfo* appInfo);

static bool CreateTaskToReady(AppInfo* appInfo);

static void InitIdleTask();

static void InitInitTask();

static void PrepareForRun(volatile Task* pTask);

static void TaskEntry();

static void Schedule();

static void KillTask();

static void ReadyToRunning();

static void RunningToReady();

static void SleepToReady();

static void WaitToReady(Queue* pWaitQueue);

static void RunningToWait(Queue* pWaitQueue);

static void RunningToSleep(u32 ms);

static bool FindTarget(ListNode* lhs, ListNode* rhs);

static TaskNode* FindTaskByName(const char* name);

static bool WaitTask(const char* name);

static bool SleepTask(u32 ms);

u32 TaskCallHandler(u32 cmd, u32 param1, u32 param2);

void TaskModuleInit();

void LaunchTask();

extern void RunTask(volatile const Task* const pTask);

extern void ClockInit();

extern void AMain();

extern u32 jiffy;

#endif //!TASK_H