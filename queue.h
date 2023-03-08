#pragma once

#include "list.h"

typedef ListNode_t QueueNode_t;

typedef struct
{
    QueueNode_t head;
    uint32_t length;
} Queue_t;

void Queue_Init(Queue_t* queue);
bool Queue_IsEmpty(Queue_t* queue);
bool Queue_IsContained(Queue_t* queue, QueueNode_t* node);
void Queue_Add(Queue_t* queue, QueueNode_t* node);
QueueNode_t* Queue_Front(Queue_t* queue);
QueueNode_t* Queue_Remove(Queue_t* queue);
uint32_t Queue_Length(Queue_t* queue);
void Queue_Rotate(Queue_t* queue);
void Queue_Destroy(Queue_t* queue, pDestroyFunc_t func);