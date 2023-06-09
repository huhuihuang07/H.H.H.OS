#include "kmain.h"

int32_t KMain(int32_t argc, char* argv[])
{
    SystemCallModuleInit();

    MutexModuleInit();

    TaskModuleInit();

    LaunchTask();

    return 0;
}
