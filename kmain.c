#include "kmain.h"
#include "kernel.h"
#include "screen.h"
#include "global.h"
#include "task.h"

int KMain(int argc, char* argv[])
{
	initTask();

	return 0;
}