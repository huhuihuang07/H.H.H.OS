#include "kmain.h"

int KMain(int argc, char* argv[])
{
    SystemCallModuleInit();

    TaskModuleInit();

    LaunchTask();

    return 0;
}
