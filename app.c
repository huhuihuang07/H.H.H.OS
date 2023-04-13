#include "app.h"
#include "syscall.h"

void AMain()
{
    uint32_t mutex = CreateMutex();

    EnterCritical(mutex);

    ExitCritical(mutex);

    DestroyMutex(mutex);
}
