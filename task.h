#ifndef TASK_H
#define TASK_H

#include <kernel.h>
#include <queue.h>

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
	u32 uid;                      // sizeof(u32) = 4
	pFunc tMain;                  // sizeof(pFunc) = 4
	Queue* wait;                  // sizeof(Queue*) = 4
	int8* stack;                  // sizeof(int8*) = 4
	char* name;                   // sizeof(char*) = 4
	u16 ldtSelector;              // sizeof(u16) = 2
	u16 total;					  // sizeof(u16) = 2
	u16 current;                  // sizeof(u16) = 2
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

static TaskNode* AppInfoToTaskNode(AppInfo* appInfo);

static void CreateTaskToReady(AppInfo* appInfo);

static void InitIdleTask();

static void InitInitTask();

static void PrepareForRun(volatile Task* pTask);

static void TaskEntry();

static void Schedule();

static void KillTask();

static void PrintTaskInfo(u32 addr);

static void ReadyToRunning();

static void RunningToReady();

static void WaitToReady(Queue* pWaitQueue);

static void RunningToWait(Queue* pWaitQueue);

static bool FindTarget(ListNode* lhs, ListNode* rhs);

static TaskNode* FindTaskByName(const char* name);

static void WaitTask(const char* name);

void TaskCallHandler(u32 cmd, u32 param1, u32 param2);

void TaskModuleInit();

void LaunchTask();

extern void RunTask(volatile const Task* const pTask);

extern void TimerInit();

extern void AMain();

#endif //!TASK_H