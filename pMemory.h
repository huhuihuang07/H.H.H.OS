#pragma once

#include "list.h"

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000 // 一页的大小 4K
#endif

#ifndef PAGE_IsValid
#define PAGE_IsValid(addr) \
    (IsEqual((uint32_t)(addr)&0xfff, 0u))
#endif

typedef enum {
    MEM_LIST_NUM = 5u,
} Mem_List_Num_t;

#define PM_ALLOC_SIZE PAGE_SIZE
#define PM_NODE_SIZE  sizeof(PMemNode_t)

typedef int8_t(PMemUnit_t)[PM_ALLOC_SIZE];

typedef struct _PMemNode_t PMemNode_t;

typedef union {
    PMemNode_t* next;
    PMemUnit_t* ptr;
} PMemUnion_t;

struct _PMemNode_t
{
    PMemUnion_t node;
    uint32_t refCount;
};

typedef struct
{
    PMemNode_t* head;
    PMemNode_t* nBase;
    PMemUnit_t* uBase;
    uint32_t max;
} PMemList_t;

#ifndef MEMORY_BASE
#define MEMORY_BASE 0x100000 // 1M，可用内存开始的位置
#endif

#define FM_ALLOC_SIZE 32u
#define FM_NODE_SIZE  sizeof(FMemNode_t)

typedef int8_t(FMemUnit_t)[FM_ALLOC_SIZE];

typedef union _FMemNode_t {
    union _FMemNode_t* next;
    FMemUnit_t* ptr;
} FMemNode_t;

typedef struct
{
    FMemNode_t* node;
    FMemNode_t* nBase;
    FMemUnit_t* uBase;
    uint32_t max;
} FMemList_t;

#define VM_HEAD_SIZE sizeof(VMemHead_t)

typedef struct
{
    ListNode_t head;
    void* ptr;
    uint32_t used;
    uint32_t free;
} VMemHead_t;

typedef List_t VMemList_t;

void pMemoryModuleInit();

void* pMalloc(size_t size);
bool pFree(const void* ptr);

void* PageAlloc(const void* ptr);
void PageFree(const void* ptr);