#include "event.h"
#include "memory.h"
#include "list.h"

static List_t* gEventList = nullptr;

typedef struct
{
    ListNode_t head;
    Event_t* event;
} EventNode_t;

void EventModuleInit()
{
    gEventList = malloc(sizeof(gEventList));

    List_Init(gEventList);
}

Event_t* CreateEvent(Event_type_t type, uint32_t id, uint32_t param1, uint32_t param2)
{
    Event_t* event = malloc(sizeof(Event_t));

    if (!IsEqual(event, nullptr))
    {
        event->type   = type;
        event->id     = id;
        event->param1 = param1;
        event->param2 = param2;

        EventNode_t* node = malloc(sizeof(EventNode_t));

        if (!IsEqual(node, nullptr))
        {
            node->event = event;

            List_Add(gEventList, StructOffset(node, EventNode_t, head));
        }
    }

    return event;
}

static bool FindTarget(ListNode_t* lhs, ListNode_t* rhs)
{
    return IsEqual(List_Node(lhs, EventNode_t, head)->event, List_Node(rhs, EventNode_t, head)->event);
}

static EventNode_t* GetEventNodeByList(Event_t* event)
{
    EventNode_t* ret = nullptr;

    EventNode_t node = {};

    node.event = event;

    ListNode_t* target = List_FindNode(gEventList, StructOffset(&node, EventNode_t, head), FindTarget);

    if (!IsEqual(target, nullptr))
    {
        ret = List_Node(target, EventNode_t, head);
    }

    return ret;
}

static bool IsEventValid(Event_t* event)
{
    return IsEqual(GetEventNodeByList(event), nullptr) ? false : true;
}

void DestroyEvent(Event_t* event)
{
    if (IsEventValid(event))
    {
        EventNode_t* node = GetEventNodeByList(event);

        List_DelNode(StructOffset(node, EventNode_t, head));

        free(node);

        free(event);
    }
}
