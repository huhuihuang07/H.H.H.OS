#pragma once

#include "type.h"

typedef enum {
    NoneEvent,
    MutexEvent,
    KeyBoEvent,
    TaskEvent
} Event_type_t;

typedef struct
{
    Event_type_t type;
    uint32_t id;
    uint32_t param1;
    uint32_t param2;
} Event_t;

void EventModuleInit();

Event_t* CreateEvent(Event_type_t type, uint32_t id, uint32_t param1, uint32_t param2);
void DestroyEvent(Event_t* event);
