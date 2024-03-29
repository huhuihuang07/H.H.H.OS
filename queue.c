#include "queue.h"

void Queue_Init(Queue_t* queue)
{
    List_Init(StructOffset(queue, Queue_t, head));

    queue->length = 0u;
}

bool Queue_IsEmpty(Queue_t* queue)
{
    return (List_IsEmpty(StructOffset(queue, Queue_t, head))) && (IsEqual(queue->length, 0u));
}

bool Queue_IsContained(Queue_t* queue, QueueNode_t* node)
{
    return List_IsContained(StructOffset(queue, Queue_t, head), node);
}

void Queue_Add(Queue_t* queue, QueueNode_t* node)
{
    if (!IsEqual(node, nullptr))
    {
        List_AddTail(StructOffset(queue, Queue_t, head), node);

        queue->length++;
    }
}

QueueNode_t* Queue_Front(Queue_t* queue)
{
    if (!Queue_IsEmpty(queue))
    {
        return queue->head.next;
    }

    return nullptr;
}

QueueNode_t* Queue_Remove(Queue_t* queue)
{
    QueueNode_t* node = nullptr;

    if (!Queue_IsEmpty(queue))
    {
        List_DelNode(node = queue->head.next);

        queue->length--;
    }

    return node;
}

uint32_t Queue_Length(Queue_t* queue)
{
    return queue->length;
}

void Queue_Rotate(Queue_t* queue)
{
    Queue_Add(queue, Queue_Remove(queue));
}

void Queue_Destroy(Queue_t* queue, pDestroyFunc_t func)
{
    List_Destroy(StructOffset(queue, Queue_t, head), func);
}