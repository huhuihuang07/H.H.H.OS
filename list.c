#include "list.h"

static void _List_Add(ListNode* node, ListNode* prev, ListNode* next)
{
    prev->next = node;
    node->prev = prev;

    next->prev = node;
    node->next = next;
}

static void _List_Del(ListNode* node, ListNode* prev, ListNode* next)
{
    prev->next = next;
    next->prev = prev;

    node->prev = node;
    node->next = node;
}

void List_Init(List* list)
{
    list->prev = list->next = list;
}

void List_Add(List* list, ListNode* node)
{
    _List_Add(node, list, list->next);
}

void List_AddTail(List* list, ListNode* node)
{
    _List_Add(node, list->prev, list);
}

void List_AddBefore(ListNode* before, ListNode* node)
{
    _List_Add(node, before->prev, before);
}

void List_AddAfter(ListNode* after, ListNode* node)
{
    _List_Add(node, after, after->next);
}

void List_DelNode(ListNode* node)
{
    _List_Del(node, node->prev, node->next);
}

void List_Replace(ListNode* old, ListNode* new)
{
    ListNode* prev = old->prev;
    ListNode* next = old->next;

    _List_Del(old, prev, next);
    _List_Add(new, prev, next);
}

bool List_IsLast(List* list, ListNode* node)
{
    return (IsEqual(list->prev, node)) && (IsEqual(list, node->next));
}

bool List_IsEmpty(List* list)
{
    return (IsEqual(list->prev, list)) && (IsEqual(list->next, list));
}

ListNode* List_FindNode(List* list, ListNode* node, pFindFunc func)
{
    ListNode* pos = nullptr;

    List_ForEach(list, pos)
    {
        if (IsEqual(func(node, pos), true))
        {
            break;
        }
    }

    return IsEqual(list, pos) ? nullptr : pos;
}

void List_Destroy(List* list, pDestroyFunc func)
{
    while (!List_IsEmpty(list))
    {
        ListNode* node = list->next;

        List_DelNode(node);

        func(node);
    }
}