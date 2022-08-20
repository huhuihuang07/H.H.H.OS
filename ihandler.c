#include <ihandler.h>
#include <screen.h>

void TimerHandler()
{
	TaskCallHandler(1, 0, 0);

	SendEOI(MASTER_EOI_PORT);
}

void DebugHandler(u32 addr)
{
	TaskCallHandler(2, addr, 0);
}

void SysCallHandler(u32 type, u32 cmd, u32 param1, u32 param2)
{
	switch(type){
		case 0 : {
			TaskCallHandler(cmd, param1, param1); 
			break;
		}
		default:
			break;
	}
}