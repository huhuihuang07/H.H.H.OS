#pragma once

#include "list.h"

#ifndef ZONE_VAILD
#define ZONE_VAILD 1 // ARDS_t 可用内存区域
#endif

#ifndef ZONE_RESERVED
#define ZONE_RESERVED 2 // ARDS_t 不可用区域
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000 // 一页的大小 4K
#endif

#ifndef PAGE_BASE
#define PAGE_BASE 0x1a000 // 104K, 页表起始位置
#endif

#ifndef PAGE_IsValid
#define PAGE_IsValid(addr) \
    (IsEqual((uint32_t)(addr)&0xfff, 0u))
#endif

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

static PMemList_t gPMemList;

#ifndef MEMORY_BASE
#define MEMORY_BASE 0x100000 // 1M，可用内存开始的位置
#endif

#define FM_ALLOC_SIZE 32
#define FM_NODE_SIZE  sizeof(FMemNode_t)
#define FM_SIZE       0x400

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

static FMemList_t gFMemList;

#define VM_HEAD_SIZE sizeof(VMemHead_t)

typedef struct
{
    ListNode_t head;
    void* ptr;
    uint32_t used;
    uint32_t free;
} VMemHead_t;

typedef List VMemList_t;

static VMemList_t gVMemList;
static VMemList_t* pVMemList;

uint32_t memoryBase;
uint32_t memorySize;

void pMemoryModuleInit();

void* pMalloc(size_t size);
bool pFree(const void* ptr);

static void FMemInit(void* mem, uint32_t size);
static void* FMemAlloc();
static bool FMemFree(const void* ptr);

static void VMemInit(void* mem, uint32_t size);
static void* VMemAlloc(size_t size);
static bool VMemFree(const void* ptr);

static void PMemInit(void* mem, uint32_t size);
void* PMemAlloc(const void* ptr);
void PMemFree(const void* ptr);