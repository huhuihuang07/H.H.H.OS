#include <stack.h>

void Stack_Init(Stack* stack)
{
	List_Init(StructOffset(stack, Stack, head));

	stack->length = 0;
}

bool Stack_IsEmpty(Stack* stack)
{
	return (List_IsEmpty(StructOffset(stack, Stack, head))) && (IsEqual(stack->length, 0));
}

bool Stack_IsContained(Stack* stack, StackNode* node)
{
	StackNode* pos = nullptr;

	List_ForEach(StructOffset(stack, Stack, head), pos){
		if(IsEqual(pos, node)){
			break;
		}
	}

	return IsEqual(pos, node);
}

void Stack_Push(Stack* stack, StackNode* node)
{
	List_Add(StructOffset(stack, Stack, head), node);

	stack->length++;
}

void Stack_Pop(Stack* stack)
{
	if(stack->length > 0){
		
		List_DelNode(stack->head.next);

		stack->length--;
	}
}

StackNode* Stack_Top(Stack* stack)
{
	if(stack->length > 0){
		return stack->head.next;
	}

	return nullptr;
}

u32 Stack_Length(Stack* stack)
{
	return stack->length;
}
