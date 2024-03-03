#include "kmain.h"

void Kernel_init()
{
    ScreenInit();

    MemoryModuleInit();

    InterruptModuleInit();

    SystemCallModuleInit();

    MutexModuleInit();

    KeyBoardModuleInit();

    TaskModuleInit();

    LaunchTask();
}

int32_t KMain(int32_t argc, char* argv[])
{
    Kernel_init();

    return 0;
}
