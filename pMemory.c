#include "pMemory.h"
#include "kernel.h"
#include "utility.h"
#include "assert.h"
#include "screen.h"

static PMemList gPMemList = {0}; // 定长分配 4K 内存页, 使用引用计数
static FMemList gFMemList = {0}; // 定长分配 32bytes 内存，不使用引用计数
static VMemList gVMemList = {0}; // 变长分配 不使用引用计数

static VMemList *pVMemList = &gVMemList;

u32 memoryBase = 0; // 堆空间基地址， 应该等于1M
u32 memorySize = 0; // 堆空间大小

static u32 pageBase = -1;
static u32 pageSize = -1;

void pMemoryModuleInit()
{
	for (u16 i = 0; !IsEqual(gMemInfo.ARDSNumber, i); ++i)
	{
		if (IsEqual(ZONE_VAILD, gMemInfo.ards[i].Type) && (gMemInfo.ards[i].LengthLow < pageSize))
		{
			pageBase = gMemInfo.ards[i].BaseAddrLow;

			pageSize = gMemInfo.ards[i].LengthLow;
		}

		if (IsEqual(ZONE_VAILD, gMemInfo.ards[i].Type) && (gMemInfo.ards[i].LengthLow > memorySize))
		{
			memoryBase = gMemInfo.ards[i].BaseAddrLow;

			memorySize = gMemInfo.ards[i].LengthLow;
		}
	}

	assert((pageBase <= PAGE_BASE < (pageBase + pageSize)));

	u32 PMemSize = (pageSize - PAGE_BASE) & ((-1) << 12);

	PMemInit((void *)(PAGE_BASE), PMemSize);

	assert(IsEqual(memoryBase, MEMORY_BASE) && PAGE_IsValid(memorySize));

	u32 FMemSize = (FM_NODE_SIZE + FM_ALLOC_SIZE) * FM_SIZE;

	assert((memorySize > FMemSize));

	FMemInit((void *)(memoryBase), FMemSize);

	u32 VMemSize = memorySize - FMemSize;

	assert((VMemSize > 0));

	void *VMemBase = (void *)(memoryBase + FMemSize);

	VMemInit(VMemBase, VMemSize);
}

static void FMemInit(void *mem, u32 size)
{
	u32 max = size / (FM_NODE_SIZE + FM_ALLOC_SIZE);

	assert((max > 0));

	gFMemList.max = max;

	FMemNode *p = gFMemList.node = gFMemList.nBase = AddrOffset(mem, 0);

	gFMemList.uBase = AddrOffset(gFMemList.nBase, gFMemList.max);

	for (u32 i = 1; !IsEqual(gFMemList.max, i); ++i)
	{
		p->next = AddrOffset(p, 1);

		p = p->next;
	}

	p->next = nullptr;
}

static void *FMemAlloc()
{
	void *ptr = nullptr;

	if (!IsEqual(gFMemList.node, nullptr))
	{
		FMemNode *alloc = gFMemList.node;

		gFMemList.node = alloc->next;

		ptr = AddrOffset(gFMemList.uBase, AddrIndex(gFMemList.nBase, alloc));

		alloc->ptr = ptr;
	}

	return ptr;
}

static bool FMemFree(const void *ptr)
{
	if (IsEqual(ptr, nullptr))
	{
		return false;
	}

	u32 index = AddrIndex(gFMemList.uBase, ptr);

	if (index >= gFMemList.max)
	{
		return false;
	}

	FMemNode *current = AddrOffset(gFMemList.nBase, index);

	if (!IsEqual(current->ptr, ptr))
	{
		return false;
	}

	current->next = gFMemList.node;

	gFMemList.node = current;

	return true;
}

static void VMemInit(void *mem, u32 size)
{
	u32 free = size - VM_HEAD_SIZE;

	assert((free > 0));

	List_Init(pVMemList);

	VMemHead *head = AddrOffset(mem, 0);

	head->ptr = AddrOffset(head, 1);

	head->used = 0;

	head->free = free;

	List_Add(pVMemList, StructOffset(head, VMemHead, head));
}

static void *VMemAlloc(size_t size)
{
	u32 allocSize = size + VM_HEAD_SIZE;

	VMemHead *alloc = nullptr;

	ListNode *pos = nullptr;

	List_ForEach(pVMemList, pos)
	{

		VMemHead *node = List_Node(pos, VMemHead, head);

		if (node->free >= allocSize)
		{

			alloc = (VMemHead *)(node->ptr + node->used + node->free - allocSize);

			alloc->head.prev = alloc->head.next = nullptr;

			alloc->ptr = AddrOffset(alloc, 1);

			alloc->used = size;

			alloc->free = 0;

			node->free -= allocSize;

			List_AddAfter(pos, StructOffset(alloc, VMemHead, head));

			break;
		}
	}

	return IsEqual(alloc, nullptr) ? nullptr : alloc->ptr;
}

static bool VMemFree(const void *ptr)
{
	if (IsEqual(ptr, nullptr))
	{
		return false;
	}

	bool ret = false;

	ListNode *pos = nullptr;

	List_ForEach(pVMemList, pos)
	{

		VMemHead *node = List_Node(pos, VMemHead, head);

		if (ret = IsEqual(node->ptr, ptr))
		{

			VMemHead *prev = List_Node(pos->prev, VMemHead, head);

			prev->free += VM_HEAD_SIZE + node->used + node->free;

			List_DelNode(pos);

			break;
		}
	}

	return ret;
}

static void PMemInit(void *mem, u32 size)
{
	u32 max = (size - PAGE_SIZE) / PAGE_SIZE;

	assert(max > 0);

	gPMemList.max = Min(max, (PAGE_SIZE / PM_NODE_SIZE));

	PMemNode *p = gPMemList.head = gPMemList.nBase = AddrOffset(mem, 0);

	gPMemList.uBase = AddrOffset(gPMemList.nBase, PAGE_SIZE / PM_NODE_SIZE);

	for (u32 i = 1; !IsEqual(i, gPMemList.max); ++i)
	{

		p->node.next = (p->refCount = 0, AddrOffset(p, 1));

		p = p->node.next;
	}

	p->node.next = (p->refCount = 0, nullptr);
}

void *PMemAlloc(const void *ptr)
{
	void *ret = nullptr;

	if (IsEqual(ptr, ret))
	{
		if (!IsEqual(gPMemList.head, nullptr))
		{
			PMemNode *node = gPMemList.head;

			u32 index = AddrIndex(gPMemList.nBase, node);

			if (index < gPMemList.max)
			{
				gPMemList.head = node->node.next;

				ret = node->node.ptr = AddrOffset(gPMemList.uBase, index);

				node->refCount = 1;
			}
		}
	}
	else
	{

		u32 index = AddrIndex(gPMemList.uBase, ptr);

		if (index < gPMemList.max)
		{
			PMemNode *node = AddrOffset(gPMemList.nBase, index);

			if (IsEqual(node->node.ptr, ptr) && (node->refCount > 0))
			{
				ret = node->node.ptr;

				++node->refCount;
			}
		}
	}

	assert(!IsEqual(ret, nullptr) && PAGE_IsValid(ret));

	return ret;
}

void PMemFree(const void *ptr)
{
	if (IsEqual(ptr, nullptr))
	{
		return;
	}

	u32 index = AddrIndex(gPMemList.uBase, ptr);

	if (index < gPMemList.max)
	{
		PMemNode *node = AddrOffset(gPMemList.nBase, index);

		if (IsEqual(node->node.ptr, ptr) && (node->refCount > 0))
		{
			--node->refCount;

			if (IsEqual(node->refCount, 0))
			{
				node->node.next = gPMemList.head;

				gPMemList.head = node;
			}
		}
	}
}

void *pMalloc(size_t size)
{
	if (IsEqual(size, 0))
	{
		return nullptr;
	}

	void *ret = nullptr;

	if (size <= FM_ALLOC_SIZE)
	{
		ret = FMemAlloc();
	}

	if (IsEqual(ret, nullptr))
	{
		ret = VMemAlloc(size);
	}

	assert(!IsEqual(ret, nullptr));

	return ret;
}

bool pFree(const void *ptr)
{
	if (IsEqual(ptr, nullptr))
	{
		return false;
	}

	bool result = FMemFree(ptr);

	if (IsEqual(result, false))
	{
		result = VMemFree(ptr);
	}

	return result;
}