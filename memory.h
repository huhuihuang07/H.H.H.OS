#ifndef MEMORY_H
#define MEMORY_H

#include <list.h>

#ifndef ZONE_VAILD
#define ZONE_VAILD 1 // ARDS 可用内存区域
#endif

#ifndef ZONE_RESERVED
#define ZONE_RESERVED 2 // ARDS 不可用区域
#endif

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000  // 一页的大小 4K
#endif 

#ifndef MEMORY_BASE
#define MEMORY_BASE 0x100000 // 1M，可用内存开始的位置
#endif

#define FM_ALLOC_SIZE 32
#define FM_NODE_SIZE  sizeof(FMemNode)
#define FM_SIZE       0x400

typedef int8 (FMemUnit)[FM_ALLOC_SIZE];

typedef union _FMemNode{
	union _FemNode* next;
	FMemUnit* ptr;
}FMemNode;

typedef struct{
	FMemNode* node;
	FMemNode* nBase;
	FMemUnit* uBase;
	u32 max;
}FMemList;

static FMemList gFMemList;

#define VM_HEAD_SIZE sizeof(VMemHead)

typedef struct{
	ListNode head;
	void* ptr;
	u32 used;
	u32 free;
}VMemHead;

typedef List VMemList;

static VMemList gVMemList;
static VMemList* pVMemList;

void MemoryModuleInit();

void* malloc(size_t size);
void free(const void* ptr);

static void FMemInit(void* mem, u32 size);
static void* FMemAlloc();
static bool FMemFree(const void* ptr);

static void VMemInit(void* mem, u32 size);
static void* VMemAlloc(size_t size);
static bool VMemFree(const void* ptr);

#endif //!MEMORY_H