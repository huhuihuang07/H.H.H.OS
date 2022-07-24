#include "ihandler.h"
#include "screen.h"

void TimerHandler()
{
	static u8 number = 0;

	number = (number + 1) % 60;

	if(0 == number){
		static u8 i = 0;

		SetPrintPos(0, 1);

		printf("Timer : ");

		putchar('a' + i);

		i = (i + 1) % 26;
	}

	SendEOI(MASTER_EOI_PORT);
}