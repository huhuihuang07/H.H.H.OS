#include "app.h"
#include "syscall.h"

void AMain()
{
    uint32_t mutex = CreateMutex();

    printf("%s mutex = %p\n", __FUNCTION__, mutex);

    EnterCritical(mutex);

    ExitCritical(mutex);

    DestroyMutex(mutex);
}
