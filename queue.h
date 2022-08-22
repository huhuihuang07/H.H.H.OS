#ifndef QUEUE_H
#define QUEUE_H

#include <list.h>

typedef ListNode QueueNode;

typedef struct{
	QueueNode head;
	u32 length;
}Queue;

void Queue_Init(Queue* queue);
bool Queue_IsEmpty(Queue* queue);
bool Queue_IsContained(Queue* queue, QueueNode* node);
void Queue_Add(Queue* queue, QueueNode* node);
QueueNode* Queue_Front(Queue* queue);
QueueNode* Queue_Remove(Queue* queue);
u32 Queue_Length(Queue* queue);
void Queue_Rotate(Queue* queue);
void Queue_Destroy(Queue* queue, pDestroyFunc func);

#endif //!QUEUE_H