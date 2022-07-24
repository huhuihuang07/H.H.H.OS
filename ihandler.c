#include "ihandler.h"
#include "task.h"

void TimerHandler()
{
	Schedule();

	SendEOI(MASTER_EOI_PORT);
}