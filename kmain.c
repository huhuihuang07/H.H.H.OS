#include "kmain.h"

int32_t KMain(int32_t argc, char* argv[])
{
    SystemCallModuleInit();

    TaskModuleInit();

    LaunchTask();

    return 0;
}
