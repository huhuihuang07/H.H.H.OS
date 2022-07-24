#include "task.h"
#include "kernel.h"
#include "screen.h"
#include "utility.h"

volatile Task* gCurrentTaskAddr = nullptr;

static Task a = {0};

void taskA()
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


void InitTask()
{

	SetDescValue(AddrOffset(gGdtInfo.entry, GDT_LDTIndex), (u32)(&a.ldt), sizeof(a.ldt) - 1, DA_LDT + DA_DPL0);
	SetDescValue(AddrOffset(gGdtInfo.entry, GDT_TSSIndex), (u32)(&a.tss), sizeof(a.tss) - 1, DA_386TSS + DA_DPL0);

	a.ldtSelector = GDT_LdtSelector;
	a.tssSelector = GDT_TssSelector;

	SetDescValue(AddrOffset(a.ldt, LDT_Code32Index), 0, 0xfffff, DA_32 + DA_C + DA_LIMIT_4K + DA_DPL3);
	SetDescValue(AddrOffset(a.ldt, LDT_Data32Index), 0, 0xfffff, DA_32 + DA_DRWA + DA_LIMIT_4K + DA_DPL3);
	SetDescValue(AddrOffset(a.ldt, LDT_Stack32Index), (u32)(&a.stack), (u32)(&a.stack) + sizeof(a.stack), DA_32 + DA_DRW + DA_DPL3);

	a.rv.gs = GDT_Video32Selector;

	a.rv.fs = LDT_Data32Selector;
	a.rv.es = LDT_Data32Selector;
	a.rv.ds = LDT_Data32Selector;

	a.rv.ss = LDT_Stack32Selector;

	a.rv.esp = (u32)(&a.stack) + sizeof(a.stack);

	a.rv.cs = LDT_Code32Selector;
	a.rv.eip = (u32)(taskA);

	a.rv.eflags = 0x3202;

	a.tss.ss0 = GDT_FlatModeDataSelector;
	a.tss.esp0 = ((u32)&a.rv) + sizeof(a.rv);
	a.tss.iomb = sizeof(a.tss);
} 

void TaskModuleInit()
{
	InitTask();
}

void LaunchTask()
{
	TimerInit();

	gCurrentTaskAddr = &a;

	RunTask(gCurrentTaskAddr);
}