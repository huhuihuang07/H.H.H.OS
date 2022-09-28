#include "kmain.h"
#include "utility.h"
#include "memory.h"

int KMain(int argc, char* argv[])
{
	for(int i = 0; !IsEqual(argc, i); ++i)
	{
		printf("argv : %s\n", argv[i]);
	}

	SystemCallModuleInit();

	TaskModuleInit();

	LaunchTask();

	return 0;
}