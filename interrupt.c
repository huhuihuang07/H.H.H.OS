#include "interrupt.h"
#include "const.h"
#include "handler.h"
#include "8259A.h"
#include "screen.h"

bool SetInterruptGate(u8 InterruptVectorNumber, u32 Handlerfunc)
{
	return SetGateValue(gIdtInfo.entry + InterruptVectorNumber, GDT_FlatModeCodeSelector, Handlerfunc, 0, DA_386IGate + DA_DPL3);
}

bool InitInterruptGate()
{
	bool ret = true;

	for(u16 i = 0; (i != gIdtInfo.size) && ret; ++i)
	{
		ret = SetInterruptGate(i, DefaultHandlerEntry);
	}

	return ret;
}

void InitInterrupt()
{
	InitInterruptGate();

	Init8259A();
}