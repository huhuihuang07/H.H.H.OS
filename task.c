#include "task.h"
#include "kernel.h"
#include "screen.h"
#include "utility.h"

volatile Task* gCurrentTaskAddr = nullptr;

static TSS gTSS = {0};

static Task a = {0};

static Task b = {0};

void TaskA()
{
	static u32 i = 0;

	while(true){

		SetPrintPos(0, 0);

		printf("This is TaskA : ");

		putchar('A' + i);

		i = (i + 1) % 26;

		Delay(1);
	}
}

void TaskB()
{
	static u32 i = 0;

	while(true){

		SetPrintPos(0, 1);

		printf("This is TaskB : ");

		putchar('0' + i);

		i = (i + 1) % 10;

		Delay(1);
	}
}

static void InitTSS()
{
	SetDescValue(AddrOffset(gGdtInfo.entry, GDT_TSSIndex), (u32)(&gTSS), sizeof(TSS) - 1, DA_386TSS + DA_DPL0);

	gTSS.ss0 = GDT_FlatModeDataSelector;
	gTSS.esp0 = BaseOfBoot;
	gTSS.iomb = sizeof(TSS);

	u16 ax = GDT_TssSelector;

	asm volatile(
		"movw %0, %%ax\n"
		"ltr      %%ax\n"
		:
		: "r"(ax)
		: "ax"
	);
}

static void PrepareForRun(volatile Task* pTask)
{
	gTSS.esp0 = (u32)(StructOffset(pTask, Task, rv)) + sizeof(RegisterValue);

	SetDescValue(AddrOffset(gGdtInfo.entry, GDT_LDTIndex), (u32)(StructOffset(pTask, Task, ldt)), sizeof(pTask->ldt) - 1, DA_LDT + DA_DPL0);

	asm volatile(
		"movw %0, %%ax\n"
		"lldt     %%ax\n"
		:
		: "r"(pTask->ldtSelector)
		: "ax"
	);
}

static void InitTask(Task* pTask, pFunc enctry)
{
	SetDescValue(AddrOffset(pTask->ldt, LDT_Code32Index), 0, 0xfffff, DA_32 + DA_C + DA_LIMIT_4K + DA_DPL3);
	SetDescValue(AddrOffset(pTask->ldt, LDT_Data32Index), 0, 0xfffff, DA_32 + DA_DRWA + DA_LIMIT_4K + DA_DPL3);
	SetDescValue(AddrOffset(pTask->ldt, LDT_Stack32Index), (u32)(StructOffset(pTask, Task, stack)), (u32)(StructOffset(pTask, Task, stack)) + sizeof(pTask->stack), DA_32 + DA_DRW + DA_DPL3);

	pTask->rv.gs = GDT_Video32Selector;

	pTask->rv.fs = LDT_Data32Selector;
	pTask->rv.es = LDT_Data32Selector;
	pTask->rv.ds = LDT_Data32Selector;

	pTask->rv.ss = LDT_Stack32Selector;

	pTask->rv.esp = (u32)(StructOffset(pTask, Task, stack)) + sizeof(pTask->stack);

	pTask->rv.cs = LDT_Code32Selector;
	pTask->rv.eip = (u32)(enctry);

	pTask->rv.eflags = 0x3202;

	pTask->ldtSelector = GDT_LdtSelector;
}

void TaskModuleInit()
{
	InitTSS();

	InitTask(&a, TaskA);

	InitTask(&b, TaskB);
}

void LaunchTask()
{
	TimerInit();

	gCurrentTaskAddr = &a;

	PrepareForRun(gCurrentTaskAddr);

	RunTask(gCurrentTaskAddr);
}

void Schedule()
{
	gCurrentTaskAddr = gCurrentTaskAddr == &a ? &b : &a;

	PrepareForRun(gCurrentTaskAddr);
}