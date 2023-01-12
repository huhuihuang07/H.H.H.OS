#include "kmain.h"
#include "utility.h"
#include "memory.h"

int KMain(int argc, char* argv[])
{
	SystemCallModuleInit();

	TaskModuleInit();

	LaunchTask();

	return 0;
}
