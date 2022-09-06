#ifndef STACK_H
#define STACK_H

#include "list.h"

typedef ListNode StackNode;

typedef struct{
	StackNode head;
	u32 length;
}Stack;

void Stack_Init(Stack* stack);
bool Stack_IsEmpty(Stack* stack);
bool Stack_IsContained(Stack* stack, StackNode* node);
void Stack_Push(Stack* stack, StackNode* node);
void Stack_Pop(Stack* stack);
StackNode* Stack_Top(Stack* stack);
u32 Stack_Length(Stack* stack);
void Stack_Destroy(Stack* stack, pDestroyFunc func);

#endif //!STACK_H