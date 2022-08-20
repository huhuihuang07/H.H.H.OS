#ifndef TASK_H
#define TASK_H

#include <kernel.h>
#include <queue.h>

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
	RegisterValue rv;   // sizeof(u32) * 18 = 4 * 18 = 72
	Descriptor ldt[3];  // sizeof(Descriptor) * 3 = 8 * 3 = 24
	u32 id;             // 4
	pFunc tMain;        // sizeof(pFunc) = 4
	int8* stack;        // sizeof(int8*) = 4
	u16 ldtSelector;    // 2
	char name[8];       // sizeof(char) * 8 = 8
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

static void InitTask(Task* pTask, pFunc entry);

static void InitIdleTask();

static void PrepareForRun(volatile Task* pTask);

static void TaskEntry();

static void Schedule();

static void KillTask();

static void PrintTaskInfo(u32 addr);

void TaskCallHandler(u32 cmd, u32 param1, u32 param2);

void TaskModuleInit();

void LaunchTask();

extern void RunTask(volatile const Task* const pTask);

extern void TimerInit();

#endif //!TASK_H