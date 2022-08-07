#include <kmain.h>
#include <interrupt.h>
#include <syscall.h>
#include <task.h>

int KMain(int argc, char* argv[])
{
	for(int i = 0; i != argc; ++i){
		printf("argv : %s\n", argv[i]);
	}

	InterruptModuleInit();

	SystemCallModuleInit();

	TaskModuleInit();

	LaunchTask();

	return 0;
}