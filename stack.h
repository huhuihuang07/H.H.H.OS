#pragma once

#include "list.h"

typedef ListNode_t StackNode_t;

typedef struct
{
    StackNode_t head;
    uint32_t length;
} Stack_t;

void Stack_Init(Stack_t* stack);
bool Stack_IsEmpty(Stack_t* stack);
bool Stack_IsContained(Stack_t* stack, StackNode_t* node);
void Stack_Push(Stack_t* stack, StackNode_t* node);
void Stack_Pop(Stack_t* stack);
StackNode_t* Stack_Top(Stack_t* stack);
uint32_t Stack_Length(Stack_t* stack);
void Stack_Destroy(Stack_t* stack, pDestroyFunc_t func);