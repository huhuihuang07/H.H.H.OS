#include <kmain.h>
#include <interrupt.h>
#include <task.h>

int KMain(int argc, char* argv[])
{
	for(int i = 0; i != argc; ++i){
		printf("argv : %s\n", argv[i]);
	}

	InterruptModuleInit();

	TaskModuleInit();

	LaunchTask();

	return 0;
}