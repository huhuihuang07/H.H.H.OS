#include "vMemory.h"
#include "assert.h"
#include "string.h"
#include "screen.h"

static void SetPageEntry(page_entry_t* entry, uint32_t index)
{
    memset(entry, 0u, sizeof(*entry));

    entry->present = 1u;
    entry->write   = 1u;
    entry->user    = 1u;
    entry->index   = index;
}

static page_entry_t* GetPDE()
{
    return (page_entry_t*)(0x3ff << 22u | 0x3ff << 12u | 0u);
}

static page_entry_t* GetPTE(uint32_t vAddr, bool create)
{
    page_entry_t* ret = AddrOffset(GetPDE(), PDEIndex(vAddr));

    assert(IsEqual(create, true) || (IsEqual(create, false) && IsEqual(ret->present, 1)));

    if (IsEqual(ret->present, 0u))
    {
        page_entry_t* pte = (page_entry_t*)PMemAlloc(nullptr);

        memset(pte, 0u, PAGE_SIZE);

        SetPageEntry(ret, PGEIndex(pte));
    }

    return (page_entry_t*)(0x3ff << 22u | (PDEIndex(vAddr) << 12u) | 0u);
}

void* CreatePDE()
{
    page_entry_t* pde = (page_entry_t*)PMemAlloc(nullptr);

    memset(pde, 0u, PAGE_SIZE);

    for (uint32_t i = 0; !IsEqual(i, 1u); ++i)
    {
        page_entry_t* pte = (page_entry_t*)PMemAlloc(nullptr);

        memset(pte, 0u, PAGE_SIZE);

        for (uint32_t j = 1; !IsEqual(j, PAGE_MAX); ++j)
        {
            SetPageEntry(AddrOffset(pte, j), Index(i, j));
        }

        SetPageEntry(AddrOffset(pde, i), PGEIndex(pte));
    }

    SetPageEntry(AddrOffset(pde, PAGE_MAX - 1u), PGEIndex(pde));

    return (void*)(pde);
}

static void FlushTLB(uint32_t vaddr)
{
    asm volatile(
        "invlpg (%0)"
        :
        : "r"(vaddr)
        : "memory");
}

static void EnablePage()
{
    asm volatile(
        "movl %%cr0,      %%eax\n"
        "orl $0x80000000, %%eax\n"
        "movl %%eax,      %%cr0\n"
        :
        :
        : "eax");
}

static void DisablePage()
{
    asm volatile(
        "movl %%cr0,      %%eax\n"
        "andl $0x7fffffff,%%eax\n"
        "movl %%eax,      %%cr0\n"
        :
        :
        : "eax");
}

static uint32_t GetCr3()
{
    uint32_t pde = 0u;

    asm volatile(
        "movl %%cr3, %%eax\n"
        "movl %%eax,    %0\n"
        : "=r"(pde)
        :
        : "eax");

    return pde;
}

static void SetCr3(uint32_t pde)
{
    if (PAGE_IsValid(pde))
    {
        asm volatile(
            "movl %0,    %%eax\n"
            "movl %%eax, %%cr3\n"
            :
            : "r"(pde)
            : "eax");
    }
}

static uint32_t GetCr2()
{
    uint32_t vAddr = 0u;

    asm volatile(
        "movl %%cr2, %%eax\n"
        "movl %%eax,    %0\n"
        : "=r"(vAddr)
        :
        : "eax");

    return vAddr;
}

void PageFault(uint32_t error)
{
    page_error_code_t* pError = (page_error_code_t*)(&error);

    if (IsEqual(pError->present, 0u))
    {
        uint32_t vAddr = GetCr2();

        uint32_t pAddr = vAddr > (memoryBase + memorySize) ? (uint32_t)(PMemAlloc(nullptr)) : vAddr;

        LinkPage(vAddr, pAddr);
    }
    else
    {
        print("%s error : %p ", __FUNCTION__, error);
    }
}

void LinkPage(uint32_t vAddr, uint32_t pAddr)
{
    SetPageEntry(AddrOffset(GetPTE(vAddr, true), PTEIndex(vAddr)), PGEIndex(pAddr));

    FlushTLB(vAddr);
}

void UnLinkPage(uint32_t vAddr)
{
    page_entry_t* pte = GetPTE(vAddr, false);

    page_entry_t* entry = AddrOffset(pte, PTEIndex(vAddr));

    assert(IsEqual(entry->present, 1u));

    memset(entry, 0u, sizeof(*entry));

    bool clear = true;

    for (int32_t i = 0; !IsEqual(i, PAGE_MAX) && IsEqual(clear, true); ++i)
    {
        entry = (page_entry_t*)(AddrOffset(pte, i));

        clear = !IsEqual(entry->present, 1u);
    }

    if (IsEqual(clear, true))
    {
        PMemFree(pte);

        page_entry_t* pde = GetPDE();

        entry = AddrOffset(pde, PDEIndex(vAddr));

        memset(entry, 0u, sizeof(*entry));
    }
}

void vMemoryModuleInit()
{
    PageFaultInit();

    SetCr3((uint32_t)(CreatePDE()));

    EnablePage();
}
