#include "stack.h"

void Stack_Init(Stack_t* stack)
{
    List_Init(StructOffset(stack, Stack_t, head));

    stack->length = 0u;
}

bool Stack_IsEmpty(Stack_t* stack)
{
    return (List_IsEmpty(StructOffset(stack, Stack_t, head))) && (IsEqual(stack->length, 0u));
}

bool Stack_IsContained(Stack_t* stack, StackNode_t* node)
{
    StackNode_t* pos = nullptr;

    List_ForEach(StructOffset(stack, Stack_t, head), pos)
    {
        if (IsEqual(pos, node))
        {
            break;
        }
    }

    return IsEqual(pos, node);
}

void Stack_Push(Stack_t* stack, StackNode_t* node)
{
    List_Add(StructOffset(stack, Stack_t, head), node);

    stack->length++;
}

void Stack_Pop(Stack_t* stack)
{
    if (!Stack_IsEmpty(stack))
    {
        List_DelNode(stack->head.next);

        stack->length--;
    }
}

StackNode_t* Stack_Top(Stack_t* stack)
{
    if (!Stack_IsEmpty(stack))
    {
        return stack->head.next;
    }

    return nullptr;
}

uint32_t Stack_Length(Stack_t* stack)
{
    return stack->length;
}

void Stack_Destroy(Stack_t* stack, pDestroyFunc_t func)
{
    List_Destroy(StructOffset(stack, Stack_t, head), func);
}
