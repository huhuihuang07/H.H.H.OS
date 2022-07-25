#include <kmain.h>
#include <interrupt.h>
#include <task.h>

int KMain(int argc, char* argv[])
{
	InterruptModuleInit();

	TaskModuleInit();

	LaunchTask();

	return 0;
}