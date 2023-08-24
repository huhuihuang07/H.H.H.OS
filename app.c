#include "app.h"
#include "syscall.h"

static uint32_t mutex = 0u;

static volatile uint32_t i = 0u;

int createTask(int argc, const char* argv[], const char* env[])
{
    mutex = CreateMutex();
}

int destroyTask(int argc, const char* argv[], const char* env[])
{
    DestroyMutex(mutex);
}

int Task(int argc, const char* argv[], const char* env[])
{
    for (uint32_t j = 0; !IsEqual(j, 10000u); j++)
    {
        EnterCritical(mutex);

        i++;

        ExitCritical(mutex);
    }
}

int AMain(int argc, const char* argv[], const char* env[])
{
    RegisterApp("create task", createTask, 255);

    Wait("create task");

    const char* tasksName[] = {"TaskA", "TaskB", "TaskC", "TaskD"};

    for (uint16_t j = 0; !IsEqual(j, ArraySize(tasksName)); j++)
    {
        RegisterApp(tasksName[j], Task, 250);
    }

    for (uint16_t j = 0; !IsEqual(j, ArraySize(tasksName)); j++)
    {
        Wait(tasksName[j]);
    }

    RegisterApp("destroy task", destroyTask, 255);

    Wait("destroy task");

    printf("i = %d", i);

    return 0;
}
