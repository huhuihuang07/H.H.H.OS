#include "kmain.h"

int32_t KMain(int32_t argc, char* argv[])
{
    SystemCallModuleInit();

    MutexModuleInit();

    KeyBoardModuleInit();

    TaskModuleInit();

    LaunchTask();

    return 0;
}
