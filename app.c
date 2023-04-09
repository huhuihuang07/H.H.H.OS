#include "app.h"
#include "syscall.h"

void AMain()
{
    uint32_t mutex = CreateMutex();

    mutex = 0x12345678;

    printf("%s mutex = %p\n", __FUNCTION__, mutex);

    EnterCritical(mutex);

    ExitCritical(mutex);

    DestroyMutex(mutex);
}
