#include "handler.h"
#include "screen.h"
#include "8259A.h"

void TimerHandler()
{
	static u8 number = 0;

	number = (number + 1) % 60;

	if(0 == number){
		static u8 i = 0;

		SetPrintPos(0, 1);

		printf("TimerHandler : ");

		putchar('a' + i);

		i = (i + 1) % 26;
	}

	SendEOI(MASTER_EOI_PORT);
}

void TimerInit()
{
	SetInterruptGate(TimerInterrupt, TimerHandlerEntry);

	u8 al = ReadIMR(MASTER_IMR_PORT);

	al &= TimerMasterIMR;

	WriteIMR(MASTER_IMR_PORT, al);

	al = ReadIMR(SLAVE_IMR_PORT);

	al &= TimerSlaveIMR;

	WriteIMR(SLAVE_IMR_PORT, al);
}