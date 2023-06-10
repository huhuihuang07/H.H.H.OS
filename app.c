#include "app.h"
#include "syscall.h"

static uint32_t mutex = 0u;

static volatile uint32_t i = 0u;

void createTask()
{
    mutex = CreateMutex();
}

void destroyTask()
{
    DestroyMutex(mutex);
}

void Task()
{
    for (uint32_t j = 0; !IsEqual(j, 1000000u); j++)
    {
        EnterCritical(mutex);

        i++;

        ExitCritical(mutex);
    }
}

void AMain()
{
    RegisterApp("create task", createTask, 255);

    Waitting("create task");

    const char* tasksName[] = {"TaskA", "TaskB", "TaskC", "TaskD"};

    for (uint16_t j = 0; !IsEqual(j, ArraySize(tasksName)); j++)
    {
        RegisterApp(tasksName[j], Task, 250);
    }

    for (uint16_t j = 0; !IsEqual(j, ArraySize(tasksName)); j++)
    {
        Waitting(tasksName[j]);
    }

    RegisterApp("destroy task", destroyTask, 255);

    Waitting("destroy task");

    printf("i = %d", i);
}
