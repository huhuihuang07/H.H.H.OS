#include "queue.h"

void Queue_Init(Queue* queue)
{
	List_Init(StructOffset(queue, Queue, head));

	queue->length = 0;
}

bool Queue_IsEmpty(Queue* queue)
{
	return (List_IsEmpty(StructOffset(queue, Queue, head))) && (IsEqual(queue->length, 0));
}

bool Queue_IsContained(Queue* queue, QueueNode* node)
{
	QueueNode* pos = nullptr;

	List_ForEach(StructOffset(queue, Queue, head), pos){
		if(IsEqual(pos, node)){
			break;
		}
	}

	return IsEqual(pos, node);
}

void Queue_Add(Queue* queue, QueueNode* node)
{
	if(!IsEqual(node, nullptr)){
		
		List_AddTail(StructOffset(queue, Queue, head), node);

		queue->length++;
	}
}

QueueNode* Queue_Front(Queue* queue)
{
	if(IsEqual(Queue_IsEmpty(queue), false)){

		return queue->head.next;
	}

	return nullptr;
}

QueueNode* Queue_Remove(Queue* queue)
{
	QueueNode* node = nullptr;

	if(IsEqual(Queue_IsEmpty(queue), false)){

		List_DelNode(node = queue->head.next);

		queue->length--;
	}

	return node;
}

u32 Queue_Length(Queue* queue)
{
	return queue->length;
}

void Queue_Rotate(Queue* queue)
{
	Queue_Add(queue, Queue_Remove(queue));
}

void Queue_Destroy(Queue* queue, pDestroyFunc func)
{
	List_Destroy(StructOffset(queue, Queue, head), func);
}