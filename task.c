#include "task.h"
#include "global.h"
#include "kernel.h"
#include "screen.h"

static Task a = {0};

static void Delay()
{

	for(int i = 0; i != 1000; ++i){
		for(int j = 0; j != 1000; ++j){
			asm volatile("nop");
		}
	}

}

void taskA()
{
	static u8 i = 0;

	while(true){

		SetPrintPos(0, 0);

		printf("This is Task A : ");

		putchar('A' + i);

		i = (i + 1) % 26;

		Delay();
	}
}


void initTask()
{

	setDescValue(gGdtInfo.entry + GDT_LDTIndex, &a.ldt, sizeof(a.ldt) - 1, DA_LDT + DA_DPL0);
	setDescValue(gGdtInfo.entry + GDT_TSSIndex, &a.tss, sizeof(a.tss) - 1, DA_386TSS + DA_DPL0);

	a.ldtSelector = GDT_LdtSelector;
	a.tssSelector = GDT_TssSelector;

	setDescValue(a.ldt + LDT_Code32Index, 0, 0xfffff, DA_32 + DA_C + DA_LIMIT_4K + DA_DPL3);
	setDescValue(a.ldt + LDT_Data32Index, 0, 0xfffff, DA_32 + DA_DRWA + DA_LIMIT_4K + DA_DPL3);
	setDescValue(a.ldt + LDT_Stack32Index, &a.stack,  &a.stack + sizeof(a.stack), DA_32 + DA_DRW + DA_DPL3);

	a.rv.gs = GDT_Video32Selector;

	a.rv.fs = LDT_Data32Selector;
	a.rv.es = LDT_Data32Selector;
	a.rv.ds = LDT_Data32Selector;

	a.rv.ss = LDT_Stack32Selector;

	a.rv.esp = &a.stack + sizeof(a.stack);

	a.rv.cs = LDT_Code32Selector;
	a.rv.eip = taskA;

	a.rv.eflags = 0x3002;

	a.tss.ss0 = GDT_FlatModeDataSelector;
	a.tss.esp0 = &a.rv + sizeof(a.rv);
	a.tss.iomb = sizeof(a.tss);

	RunTask(&a);
}