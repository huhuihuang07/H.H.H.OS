#include "ihandler.h"
#include "syscall.h"
#include "screen.h"

void TimerHandler()
{
	TaskCallHandler(1, 0, 0);

	SendEOI(MASTER_EOI_PORT);
}

u32 DebugHandler(u32 addr)
{
	u32 ret = TaskCallHandler(2, addr, 0);	

	return ret;
}

void PageFaultHandler(u32 error)
{
	PageFault(error);
}

void DefaultInterruptHandler()
{
	printf("%s\n", __FUNCTION__);
}

void DefaultFaultHandler(u32 error)
{
	printf("%s ==> error : %p\n", __FUNCTION__, error);
}

u32 SysCallHandler(u32 type, u32 cmd, u32 param1, u32 param2)
{
	u32 ret = 0;

	switch(type){
		case SysCall_Task : {
			ret = TaskCallHandler(cmd, param1, param2); 
			break;
		}
		default:
			break;
	}

	return ret;
}