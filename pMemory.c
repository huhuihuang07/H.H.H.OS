#include "pMemory.h"
#include "kernel.h"
#include "utility.h"
#include "assert.h"
#include "screen.h"

static PMemList_t gPMemList[MEM_LIST_NUM] = {0}; // 定长分配 4K 内存页, 使用引用计数
static FMemList_t gFMemList[MEM_LIST_NUM] = {0}; // 定长分配 32bytes 内存，不使用引用计数
static VMemList_t gVMemList[MEM_LIST_NUM] = {0}; // 变长分配 不使用引用计数

static void FMemInit(FMemList_t* pMemList, void* mem, uint32_t size)
{
    print("FMemInit: %p, %p\n", mem, size);

    uint32_t max = RoundDown(size, (FM_NODE_SIZE + FM_ALLOC_SIZE)) / (FM_NODE_SIZE + FM_ALLOC_SIZE);

    assert((max > 0u));

    pMemList->max = max;

    FMemNode_t* p = pMemList->node = pMemList->nBase = AddrOffset(mem, 0u);

    pMemList->uBase = AddrOffset(pMemList->nBase, pMemList->max);

    for (uint32_t i = 1u; !IsEqual(pMemList->max, i); ++i)
    {
        p->next = AddrOffset(p, 1u);

        p = p->next;
    }

    p->next = nullptr;
}

static void* FMemAlloc(FMemList_t* pMemList)
{
    void* ptr = nullptr;

    if (!IsEqual(pMemList->node, nullptr))
    {
        FMemNode_t* alloc = pMemList->node;

        pMemList->node = alloc->next;

        ptr = AddrOffset(pMemList->uBase, AddrIndex(pMemList->nBase, alloc));

        alloc->ptr = ptr;
    }

    return ptr;
}

static bool FMemFree(FMemList_t* pMemList, const void* ptr)
{
    if (IsEqual(ptr, nullptr))
    {
        return false;
    }

    uint32_t index = AddrIndex(pMemList->uBase, ptr);

    if (index >= pMemList->max)
    {
        return false;
    }

    FMemNode_t* current = AddrOffset(pMemList->nBase, index);

    if (!IsEqual(current->ptr, ptr))
    {
        return false;
    }

    current->next = pMemList->node;

    pMemList->node = current;

    return true;
}

static void VMemInit(VMemList_t* pMemList, void* mem, uint32_t size)
{
    print("VMemInit: %p, %p\n", mem, size);

    uint32_t free = size - VM_HEAD_SIZE;

    assert((free > 0u));

    List_Init(pMemList);

    VMemHead_t* head = AddrOffset(mem, 0u);

    head->ptr = AddrOffset(head, 1u);

    head->used = 0u;

    head->free = free;

    List_Add(pMemList, StructOffset(head, VMemHead_t, head));
}

static void* VMemAlloc(VMemList_t* pMemList, size_t size)
{
    uint32_t allocSize = size + VM_HEAD_SIZE;

    VMemHead_t* alloc = nullptr;

    ListNode_t* pos = nullptr;

    List_ForEach(pMemList, pos)
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

static bool VMemFree(VMemList_t* pMemList, const void* ptr)
{
    if (IsEqual(ptr, nullptr))
    {
        return false;
    }

    bool ret = false;

    ListNode_t* pos = nullptr;

    List_ForEach(pMemList, pos)
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

static void PMemInit(PMemList_t* pMemList, void* mem, uint32_t size)
{
    print("PMemInit: %p, %p\n", mem, size);

    uint32_t max = RoundDown(size, PAGE_SIZE) / PAGE_SIZE;

    assert(max > 0u);

    pMemList->max = max;

    pMemList->uBase = mem;

    PMemNode_t* p = pMemList->head = pMemList->nBase = pMalloc(max * PM_NODE_SIZE);

    for (uint32_t i = 1; !IsEqual(i, pMemList->max); ++i)
    {
        p->node.next = (p->refCount = 0u, AddrOffset(p, 1u));

        p = p->node.next;
    }

    p->node.next = (p->refCount = 0u, nullptr);
}

static void* PMemAlloc(PMemList_t* pMemList, const void* ptr)
{
    void* ret = nullptr;

    if (IsEqual(ptr, ret))
    {
        if (!IsEqual(pMemList->head, nullptr))
        {
            PMemNode_t* node = pMemList->head;

            uint32_t index = AddrIndex(pMemList->nBase, node);

            if (index < pMemList->max)
            {
                pMemList->head = node->node.next;

                ret = node->node.ptr = AddrOffset(pMemList->uBase, index);

                node->refCount = 1u;
            }
        }
    }
    else
    {
        uint32_t index = AddrIndex(pMemList->uBase, ptr);

        if (index < pMemList->max)
        {
            PMemNode_t* node = AddrOffset(pMemList->nBase, index);

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

static bool PMemFree(PMemList_t* pMemList, const void* ptr)
{
    if (IsEqual(ptr, nullptr))
    {
        return false;
    }

    bool result = false;

    uint32_t index = AddrIndex(pMemList->uBase, ptr);

    if (index < pMemList->max)
    {
        PMemNode_t* node = AddrOffset(pMemList->nBase, index);

        if (result = (IsEqual(node->node.ptr, ptr) && (node->refCount > 0u)))
        {
            --node->refCount;

            if (IsEqual(node->refCount, 0u))
            {
                node->node.next = pMemList->head;

                pMemList->head = node;
            }
        }
    }

    return result;
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
        for (int8_t i = 0u; !IsEqual(i, MEM_LIST_NUM) && IsEqual(ret, nullptr); ++i)
        {
            ret = FMemAlloc(AddrOffset(gFMemList, i));
        }
    }

    if (IsEqual(ret, nullptr))
    {
        for (int8_t i = 0u; !IsEqual(i, MEM_LIST_NUM) && IsEqual(ret, nullptr); ++i)
        {
            ret = VMemAlloc(AddrOffset(gVMemList, i), size);
        }
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

    bool result = false;

    for (int8_t i = 0u; !IsEqual(i, MEM_LIST_NUM) && IsEqual(result, false); ++i)
    {
        result = FMemFree(AddrOffset(gFMemList, i), ptr);
    }

    if (IsEqual(result, false))
    {
        for (int8_t i = 0u; !IsEqual(i, MEM_LIST_NUM) && IsEqual(result, nullptr); ++i)
        {
            result = VMemFree(AddrOffset(gVMemList, i), ptr);
        }
    }

    return result;
}

void* PageAlloc(const void* ptr)
{
    void* result = nullptr;

    for (int8_t i = 0u; !IsEqual(i, MEM_LIST_NUM) && IsEqual(result, nullptr); ++i)
    {
        result = PMemAlloc(AddrOffset(gPMemList, i), ptr);
    }

    return result;
}

void PageFree(const void* ptr)
{
    bool result = false;

    for (int8_t i = 0u; !IsEqual(i, MEM_LIST_NUM) && IsEqual(result, false); ++i)
    {
        result = PMemFree(AddrOffset(gPMemList, i), ptr);
    }
}

void pMemoryModuleInit()
{
    uint32_t pageBase = 0u, pageSize = 0u;

    VMemList_t* pVMemList = AddrOffset(gVMemList, 0u);

    FMemList_t* pFMemList = AddrOffset(gFMemList, 0u);

    for (uint16_t i = 0u; !IsEqual(gMemInfo.ARDSNumber, i); ++i)
    {
        if (!IsEqual(gMemInfo.ards[i].BaseAddrLow, 0u))
        {
            if (!IsEqual(ZONE_RESERVED, gMemInfo.ards[i].Type))
            {
                if ((gMemInfo.ards[i].LengthLow > pageSize))
                {
                    if (!IsEqual(pageSize, 0u))
                    {
                        VMemInit(pVMemList, ((void*)pageBase), pageSize);

                        pVMemList = AddrOffset(gVMemList, AddrIndex(gVMemList, pVMemList) + 1u);
                    }

                    pageBase = gMemInfo.ards[i].BaseAddrLow;

                    pageSize = gMemInfo.ards[i].LengthLow;
                }
                else
                {
                    FMemInit(pFMemList, ((void*)gMemInfo.ards[i].BaseAddrLow), gMemInfo.ards[i].LengthLow);

                    pFMemList = AddrOffset(gFMemList, AddrIndex(gFMemList, pFMemList) + 1u);
                }
            }
        }
        else
        {
            if (!IsEqual(ZONE_RESERVED, gMemInfo.ards[i].Type))
            {
                VMemInit(pVMemList, ((void*)BaseOfAPP), gMemInfo.ards[i].LengthLow - BaseOfAPP);

                pVMemList = AddrOffset(gVMemList, AddrIndex(gVMemList, pVMemList) + 1u);
            }
        }
    }

    if (!IsEqual(pageSize, 0u))
    {
        PMemInit(AddrOffset(gPMemList, 0u), ((void*)pageBase), pageSize);
    }
}