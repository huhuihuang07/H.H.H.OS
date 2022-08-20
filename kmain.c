#include <kmain.h>
#include <utility.h>

int KMain(int argc, char* argv[])
{
	for(int i = 0; !IsEqual(argc, i); ++i){
		printf("argv : %s\n", argv[i]);
	}

	InterruptModuleInit();

	SystemCallModuleInit();

	MemoryModuleInit();

	TaskModuleInit();

	LaunchTask();

	return 0;
}