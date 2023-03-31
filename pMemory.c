#include "pMemory.h"
#include "kernel.h"
#include "utility.h"
#include "assert.h"
#include "screen.h"

static PMemList_t gPMemList = {0}; // 定长分配 4K 内存页, 使用引用计数
static FMemList_t gFMemList = {0}; // 定长分配 32bytes 内存，不使用引用计数
static VMemList_t gVMemList = {0}; // 变长分配 不使用引用计数

static VMemList_t* pVMemList = &gVMemList;

uint32_t memoryBase = 0u; // 堆空间基地址， 应该等于1M
uint32_t memorySize = 0u; // 堆空间大小

static uint32_t pageBase = -1;
static uint32_t pageSize = -1;

void pMemoryModuleInit()
{
    for (uint16_t i = 0; !IsEqual(gMemInfo.ARDSNumber, i); ++i)
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

    uint32_t PMemSize = (pageSize - PAGE_BASE) & ((-1) << 12u);

    PMemInit((void*)(PAGE_BASE), PMemSize);

    assert(IsEqual(memoryBase, MEMORY_BASE) && PAGE_IsValid(memorySize));

    uint32_t FMemSize = (FM_NODE_SIZE + FM_ALLOC_SIZE) * FM_SIZE;

    assert((memorySize > FMemSize));

    FMemInit((void*)(memoryBase), FMemSize);

    uint32_t VMemSize = memorySize - FMemSize;

    assert((VMemSize > 0u));

    void* VMemBase = (void*)(memoryBase + FMemSize);

    VMemInit(VMemBase, VMemSize);
}

static void FMemInit(void* mem, uint32_t size)
{
    uint32_t max = size / (FM_NODE_SIZE + FM_ALLOC_SIZE);

    assert((max > 0u));

    gFMemList.max = max;

    FMemNode_t* p = gFMemList.node = gFMemList.nBase = AddrOffset(mem, 0u);

    gFMemList.uBase = AddrOffset(gFMemList.nBase, gFMemList.max);

    for (uint32_t i = 1; !IsEqual(gFMemList.max, i); ++i)
    {
        p->next = AddrOffset(p, 1u);

        p = p->next;
    }

    p->next = nullptr;
}

static void* FMemAlloc()
{
    void* ptr = nullptr;

    if (!IsEqual(gFMemList.node, nullptr))
    {
        FMemNode_t* alloc = gFMemList.node;

        gFMemList.node = alloc->next;

        ptr = AddrOffset(gFMemList.uBase, AddrIndex(gFMemList.nBase, alloc));

        alloc->ptr = ptr;
    }

    return ptr;
}

static bool FMemFree(const void* ptr)
{
    if (IsEqual(ptr, nullptr))
    {
        return false;
    }

    uint32_t index = AddrIndex(gFMemList.uBase, ptr);

    if (index >= gFMemList.max)
    {
        return false;
    }

    FMemNode_t* current = AddrOffset(gFMemList.nBase, index);

    if (!IsEqual(current->ptr, ptr))
    {
        return false;
    }

    current->next = gFMemList.node;

    gFMemList.node = current;

    return true;
}

static void VMemInit(void* mem, uint32_t size)
{
    uint32_t free = size - VM_HEAD_SIZE;

    assert((free > 0u));

    List_Init(pVMemList);

    VMemHead_t* head = AddrOffset(mem, 0u);

    head->ptr = AddrOffset(head, 1u);

    head->used = 0u;

    head->free = free;

    List_Add(pVMemList, StructOffset(head, VMemHead_t, head));
}

static void* VMemAlloc(size_t size)
{
    uint32_t allocSize = size + VM_HEAD_SIZE;

    VMemHead_t* alloc = nullptr;

    ListNode_t* pos = nullptr;

    List_ForEach(pVMemList, pos)
    {
        VMemHead_t* node = List_Node(pos, VMemHead_t, head);

        if (node->free >= allocSize)
        {
            alloc = (VMemHead_t*)(node->ptr + node->used + node->free - allocSize);

            alloc->head.prev = alloc->head.next = nullptr;

            alloc->ptr = AddrOffset(alloc, 1u);

            alloc->used = size;

            alloc->free = 0u;

            node->free -= allocSize;

            List_AddAfter(pos, StructOffset(alloc, VMemHead_t, head));

            break;
        }
    }

    return IsEqual(alloc, nullptr) ? nullptr : alloc->ptr;
}

static bool VMemFree(const void* ptr)
{
    if (IsEqual(ptr, nullptr))
    {
        return false;
    }

    bool ret = false;

    ListNode_t* pos = nullptr;

    List_ForEach(pVMemList, pos)
    {
        VMemHead_t* node = List_Node(pos, VMemHead_t, head);

        if (ret = IsEqual(node->ptr, ptr))
        {
            VMemHead_t* prev = List_Node(pos->prev, VMemHead_t, head);

            prev->free += VM_HEAD_SIZE + node->used + node->free;

            List_DelNode(pos);

            break;
        }
    }

    return ret;
}

static void PMemInit(void* mem, uint32_t size)
{
    uint32_t max = (size - PAGE_SIZE) / PAGE_SIZE;

    assert(max > 0u);

    gPMemList.max = Min(max, (PAGE_SIZE / PM_NODE_SIZE));

    PMemNode_t* p = gPMemList.head = gPMemList.nBase = AddrOffset(mem, 0u);

    gPMemList.uBase = AddrOffset(gPMemList.nBase, PAGE_SIZE / PM_NODE_SIZE);

    for (uint32_t i = 1; !IsEqual(i, gPMemList.max); ++i)
    {
        p->node.next = (p->refCount = 0u, AddrOffset(p, 1u));

        p = p->node.next;
    }

    p->node.next = (p->refCount = 0u, nullptr);
}

void* PMemAlloc(const void* ptr)
{
    void* ret = nullptr;

    if (IsEqual(ptr, ret))
    {
        if (!IsEqual(gPMemList.head, nullptr))
        {
            PMemNode_t* node = gPMemList.head;

            uint32_t index = AddrIndex(gPMemList.nBase, node);

            if (index < gPMemList.max)
            {
                gPMemList.head = node->node.next;

                ret = node->node.ptr = AddrOffset(gPMemList.uBase, index);

                node->refCount = 1u;
            }
        }
    }
    else
    {
        uint32_t index = AddrIndex(gPMemList.uBase, ptr);

        if (index < gPMemList.max)
        {
            PMemNode_t* node = AddrOffset(gPMemList.nBase, index);

            if (IsEqual(node->node.ptr, ptr) && (node->refCount > 0u))
            {
                ret = node->node.ptr;

                ++node->refCount;
            }
        }
    }

    assert(!IsEqual(ret, nullptr) && PAGE_IsValid(ret));

    return ret;
}

void PMemFree(const void* ptr)
{
    if (IsEqual(ptr, nullptr))
    {
        return;
    }

    uint32_t index = AddrIndex(gPMemList.uBase, ptr);

    if (index < gPMemList.max)
    {
        PMemNode_t* node = AddrOffset(gPMemList.nBase, index);

        if (IsEqual(node->node.ptr, ptr) && (node->refCount > 0u))
        {
            --node->refCount;

            if (IsEqual(node->refCount, 0u))
            {
                node->node.next = gPMemList.head;

                gPMemList.head = node;
            }
        }
    }
}

void* pMalloc(size_t size)
{
    if (IsEqual(size, 0u))
    {
        return nullptr;
    }

    void* ret = nullptr;

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

bool pFree(const void* ptr)
{
    if (IsEqual(ptr, nullptr))
    {
        return false;
    }

    bool result = FMemFree(ptr);

    if (!result)
    {
        result = VMemFree(ptr);
    }

    return result;
}