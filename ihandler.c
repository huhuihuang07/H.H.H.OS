#include <ihandler.h>
#include <screen.h>

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

u32 SysCallHandler(u32 type, u32 cmd, u32 param1, u32 param2)
{
	u32 ret = 0;

	switch(type){
		case 0 : {
			ret = TaskCallHandler(cmd, param1, param2); 
			break;
		}
		default:
			break;
	}

	return ret;
}