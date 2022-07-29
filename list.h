#ifndef LIST_H
#define LIST_H

#include <utility.h>

typedef struct _ListNode{
	struct _ListNode* prev;
	struct _ListNode* next;
}ListNode;

typedef ListNode List;

#ifndef List_ForEach
#define List_ForEach(list, pos) \
	for((pos) = ((List*)(list))->next; !IsEqual((list), (pos)); (pos) = (pos)->next)
#endif		

#ifndef List_Node
#define List_Node(ptr, type, member) \
	container_of(ptr, type, member)	
#endif

static void _List_Add(ListNode* node, ListNode* prev, ListNode* next);
static void _List_Del(ListNode* node, ListNode* prev, ListNode* next);

void List_Init(List* list);
void List_Add(List* list, ListNode* node);
void List_AddTail(List* list, ListNode* node);
void List_AddBefore(ListNode* before, ListNode* node);
void List_AddAfter(ListNode* after, ListNode* node);
void List_DelNode(ListNode* node);
void List_Replace(ListNode* old, ListNode* new);
bool List_IsLast(List* list, ListNode* node);		
bool List_IsEmpty(List* list);

#endif //!LIST_H