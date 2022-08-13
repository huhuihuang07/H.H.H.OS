#include <memory.h>
#include <kernel.h>
#include <utility.h>
#include <assert.h>

static FMemList gFMemList = {0};
static VMemList gVMemList = {0};

static VMemList* pVMemList = &gVMemList;

static u32 memoryBase = 0; // 可用内存基地址， 应该等于1M
static u32 memorySize = 0; // 可用内存大小

void MemoryModuleInit()
{
	for(u16 i = 0; !IsEqual(gMemInfo.ARDSNumber, i); ++i)
	{
		if(IsEqual(ZONE_VAILD, gMemInfo.ards[i].Type) && (gMemInfo.ards[i].LengthLow > memorySize))
		{
			memoryBase = gMemInfo.ards[i].BaseAddrLow;

			memorySize = gMemInfo.ards[i].LengthLow;
		}

	}

	assert(IsEqual(memoryBase, MEMORY_BASE));

	assert(IsEqual(memorySize & 0xfff, 0));

	u32 FMemSize = (FM_NODE_SIZE + FM_ALLOC_SIZE) * FM_SIZE;

	assert((memorySize > FMemSize));

	FMemInit((void*)(memoryBase), FMemSize);

	u32 VMemSize = memorySize - FMemSize;

	assert((VMemSize > 0));

	void* VMemBase = (void*)(memoryBase + FMemSize);

	VMemInit(VMemBase, VMemSize);
}

static void FMemInit(void* mem, u32 size)
{
	u32 max = size / (FM_NODE_SIZE + FM_ALLOC_SIZE);
	
	assert((max > 0));

	gFMemList.max = max;

	gFMemList.nBase = AddrOffset(mem, 0);

	gFMemList.uBase = AddrOffset(gFMemList.nBase, gFMemList.max);

	FMemNode* p = gFMemList.node = gFMemList.nBase;

	for(u32 i = 1; !IsEqual(gFMemList.max, i); ++i)
	{
		p->next = AddrOffset(p, 1);

		p = (FMemNode*)(p->next);
	}

	p->next = nullptr;
}

static void* FMemAlloc()
{
	void* ptr = nullptr;

	if(!IsEqual(gFMemList.node, nullptr))
	{
		FMemNode* alloc = gFMemList.node;

		gFMemList.node = (FMemNode*)(alloc->next);

		ptr = AddrOffset(gFMemList.uBase, AddrIndex(gFMemList.nBase, alloc));

		alloc->ptr = ptr;
	}

	return ptr;
}

static bool FMemFree(const void* ptr)
{
	if(IsEqual(ptr, nullptr)){
		return false;
	}

	u32 index = AddrIndex(gFMemList.uBase, ptr);

	if(index >= gFMemList.max){
		return false;
	}

	FMemNode* current = AddrOffset(gFMemList.nBase, index);

	if(!IsEqual(current->ptr, ptr)){
		return false;
	}

	current->next = (union _FemNode*)(gFMemList.node);

	gFMemList.node = current;

	return true;
}

static void VMemInit(void* mem, u32 size)
{
	u32 free = size - VM_HEAD_SIZE;

	assert((free > 0));

	List_Init(pVMemList);

	VMemHead* head = AddrOffset(mem, 0);

	head->ptr = AddrOffset(head, 1);

	head->used = 0;

	head->free = free;

	List_Add(pVMemList, StructOffset(head, VMemHead, head));
}

static void* VMemAlloc(size_t size)
{
	u32 allocSize = size + VM_HEAD_SIZE;

	VMemHead* alloc = nullptr;

	ListNode* pos = nullptr;

	List_ForEach(pVMemList, pos){

		VMemHead* node = List_Node(pos, VMemHead, head);

		if(node->free >= allocSize){

			alloc = (VMemHead*)(node->ptr + node->used + node->free - allocSize);

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

static bool VMemFree(const void* ptr)
{
	if(IsEqual(ptr, nullptr)){
		return false;
	}

	bool ret = false;

	ListNode* pos = nullptr;

	List_ForEach(pVMemList, pos){

		VMemHead* node = List_Node(pos, VMemHead, head);

		if(ret = IsEqual(node->ptr, ptr)){

			VMemHead* prev = List_Node(pos->prev, VMemHead, head);

			prev->free += VM_HEAD_SIZE + node->used + node->free;

			List_DelNode(pos);

			break;
		}
	}

	return ret;
}

void* malloc(size_t size)
{
	if(IsEqual(size, 0)){
		return nullptr;
	}

	void* ret = nullptr;

	if(size <= FM_ALLOC_SIZE){
		ret = FMemAlloc();
	}

	if(IsEqual(ret, nullptr)){
		ret = VMemAlloc(size);
	}

	assert(!IsEqual(ret, nullptr));

	return ret;
}

void free(const void* ptr)
{
	if(IsEqual(ptr, nullptr)){
		return;
	}

	bool result = FMemFree(ptr);

	if(IsEqual(result, false)){
		VMemFree(ptr);
	}
}