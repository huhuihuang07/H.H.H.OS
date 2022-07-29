#include <ihandler.h>
#include <task.h>

void TimerHandler()
{
	Schedule();

	SendEOI(MASTER_EOI_PORT);
}

void DebugHandler(RegisterValue registerInfo)
{
	ClearScreen();
	SetPrintPos(0, 0);
	printf("-------------------------------\n");
	printf("|=== Commom register value ===|\n");
	printf("| eax --> %p\n", registerInfo.eax);
	printf("| ebx --> %p\n", registerInfo.ebx);
	printf("| ecx --> %p\n", registerInfo.ecx);
	printf("| edx --> %p\n", registerInfo.edx);
	printf("| ebp --> %p\n", registerInfo.ebp);
	printf("| esi --> %p\n", registerInfo.esi);
	printf("| edi --> %p\n", registerInfo.edi);
	printf("|=== Segment register value ==|\n");
	printf("| ds --> 0b%b\n", registerInfo.ds);
	printf("| es --> 0b%b\n", registerInfo.es);
	printf("| fs --> 0b%b\n", registerInfo.fs);
	printf("| gs --> 0b%b\n", registerInfo.gs);
	printf("|==== Combin register value ==|\n");
	printf("| ss --> 0b%b\n", registerInfo.ss);
	printf("| esp --> %p\n", registerInfo.esp);
	printf("| eflags --> %p\n", registerInfo.eflags);
	printf("| cs --> 0b%b\n", registerInfo.cs);
	printf("| eip --> %p\n", registerInfo.eip);
	printf("|=============================|\n");
	printf("-------------------------------\n");
	while(true); // TODO
}