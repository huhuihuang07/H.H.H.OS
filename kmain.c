#include <kmain.h>

int KMain(int argc, char* argv[])
{
	for(int i = 0; i != argc; ++i){
		printf("argv : %s\n", argv[i]);
	}

	InterruptModuleInit();

	SystemCallModuleInit();

	MemoryModuleInit();

	TaskModuleInit();

	LaunchTask();

	return 0;
}