#include "list.h"

static void _List_Add(ListNode_t* node, ListNode_t* prev, ListNode_t* next)
{
    prev->next = node;
    node->prev = prev;

    next->prev = node;
    node->next = next;
}

static void _List_Del(ListNode_t* node, ListNode_t* prev, ListNode_t* next)
{
    prev->next = next;
    next->prev = prev;

    node->prev = node;
    node->next = node;
}

void List_Init(List_t* list)
{
    list->prev = list->next = list;
}

void List_Add(List_t* list, ListNode_t* node)
{
    _List_Add(node, list, list->next);
}

void List_AddTail(List_t* list, ListNode_t* node)
{
    _List_Add(node, list->prev, list);
}

void List_AddBefore(ListNode_t* before, ListNode_t* node)
{
    _List_Add(node, before->prev, before);
}

void List_AddAfter(ListNode_t* after, ListNode_t* node)
{
    _List_Add(node, after, after->next);
}

void List_DelNode(ListNode_t* node)
{
    _List_Del(node, node->prev, node->next);
}

void List_Replace(ListNode_t* old, ListNode_t* new)
{
    ListNode_t* prev = old->prev;
    ListNode_t* next = old->next;

    _List_Del(old, prev, next);
    _List_Add(new, prev, next);
}

bool List_IsLast(List_t* list, ListNode_t* node)
{
    return (IsEqual(list->prev, node)) && (IsEqual(list, node->next));
}

bool List_IsEmpty(List_t* list)
{
    return (IsEqual(list->prev, list)) && (IsEqual(list->next, list));
}

ListNode_t* List_FindNode(List_t* list, ListNode_t* node, pFindFunc_t func)
{
    ListNode_t* pos = nullptr;

    List_ForEach(list, pos)
    {
        if (func(node, pos))
        {
            break;
        }
    }

    return IsEqual(list, pos) ? nullptr : pos;
}

void List_Destroy(List_t* list, pDestroyFunc_t func)
{
    while (!List_IsEmpty(list))
    {
        ListNode_t* node = list->next;

        List_DelNode(node);

        func(node);
    }
}

bool List_IsContained(List_t* list, ListNode_t* node)
{
    bool ret = false;

    ListNode_t* pos = nullptr;

    List_ForEach(list, pos)
    {
        if (ret = IsEqual(node, pos))
        {
            break;
        }
    }

    return ret;
}