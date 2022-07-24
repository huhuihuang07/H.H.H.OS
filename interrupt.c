#include "interrupt.h"
#include "utility.h"
#include "kernel.h"
#include "8259A.h"

bool SetInterruptGate(u8 InterruptVectorNumber, u32 Handlerfunc)
{
	return SetGateValue(AddrOffset(gIdtInfo.entry, InterruptVectorNumber), GDT_FlatModeCodeSelector, Handlerfunc, 0, DA_386IGate + DA_DPL3);
}

bool InterruptGateInit()
{
	bool ret = true;

	for(u16 i = 0; (i != gIdtInfo.size) && ret; ++i)
	{
		ret = SetInterruptGate(i, (u32)(DefaultHandlerEntry));
	}

	return ret;
}

void InterruptModuleInit()
{
	InterruptGateInit();

	Init8259A();
}