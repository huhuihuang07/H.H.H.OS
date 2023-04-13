#pragma once

#include "utility.h"

typedef struct _ListNode_t
{
    struct _ListNode_t* prev;
    struct _ListNode_t* next;
} ListNode_t;

typedef ListNode_t List_t;

typedef bool (*pFindFunc_t)(ListNode_t*, ListNode_t*);
typedef void (*pDestroyFunc_t)(ListNode_t*);

#ifndef List_ForEach
#define List_ForEach(list, pos) \
    for ((pos) = ((List_t*)(list))->next; !IsEqual((list), (pos)); (pos) = (pos)->next)
#endif

#ifndef List_Node
#define List_Node(ptr, type, member) \
    container_of(ptr, type, member)
#endif

static void _List_Add(ListNode_t* node, ListNode_t* prev, ListNode_t* next);
static void _List_Del(ListNode_t* node, ListNode_t* prev, ListNode_t* next);

void List_Init(List_t* list);
void List_Add(List_t* list, ListNode_t* node);
void List_AddTail(List_t* list, ListNode_t* node);
void List_AddBefore(ListNode_t* before, ListNode_t* node);
void List_AddAfter(ListNode_t* after, ListNode_t* node);
void List_DelNode(ListNode_t* node);
void List_Replace(ListNode_t* old, ListNode_t* new);
bool List_IsLast(List_t* list, ListNode_t* node);
bool List_IsEmpty(List_t* list);
ListNode_t* List_FindNode(List_t* list, ListNode_t* node, pFindFunc_t func);
void List_Destroy(List_t* list, pDestroyFunc_t func);