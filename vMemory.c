#include "vMemory.h"
#include "assert.h"
#include "string.h"
#include "screen.h"

void vMemoryModuleInit()
{
	page_entry_t* pde = (page_entry_t*)PMemAlloc(nullptr);

	memset(pde, 0, PAGE_SIZE);

	u32 size = Min(gMemSize / PDE_SIZE, PAGE_MAX - 1);

	for(u32 i = 0; !IsEqual(i, size); ++i)
	{
		page_entry_t* pte = (page_entry_t*)PMemAlloc(nullptr);

		memset(pte, 0, PAGE_SIZE);

		for(u32 j = 1; !IsEqual(j, PAGE_MAX); ++j)
		{
			SetPageEntry(AddrOffset(pte, j), Index(i, j));
		}

		SetPageEntry(AddrOffset(pde, i), PGEIndex(pte));
	}

	SetPageEntry(AddrOffset(pde, PAGE_MAX - 1), PGEIndex(pde));

	PageFaultInit();

	SetCr3((u32)(pde));

	EnablePage();
}

static void SetPageEntry(page_entry_t* entry, u32 index)
{
	memset(entry, 0, sizeof(*entry));

    entry->present = 1;
    entry->write = 1;
    entry->user = 1;
    entry->index = index;
}

static void FlushTLB(u32 vaddr)
{
    asm volatile(
    	"invlpg (%0)" 
    	:
    	:"r"(vaddr)
        : "memory"
    );
}

static void EnablePage()
{
	asm volatile(
		"movl %%cr0,      %%eax\n"
		"orl $0x80000000, %%eax\n"
		"movl %%eax,      %%cr0\n"
		:
		:
		: "eax"
	);
}

static void DisablePage()
{
	asm volatile(
		"movl %%cr0,      %%eax\n"
		"andl $0x7fffffff,%%eax\n"
		"movl %%eax,      %%cr0\n"
		:
		:
		: "eax"
	);
}

static u32 GetCr3()
{
	u32 pde = 0;

	asm volatile(
		"movl %%cr3, %%eax\n"
		"movl %%eax,    %0\n"
		: "=r"(pde)
		:
		: "eax"
	);

	return pde;
}

static void SetCr3(u32 pde)
{
	PAGE_IsValid(pde);

	asm volatile(
		"movl %0,    %%eax\n"
		"movl %%eax, %%cr3\n"
		:
		: "r"(pde)
		: "eax"
	);
}

static u32 GetCr2()
{
	u32 vAddr = 0;

	asm volatile(
		"movl %%cr2, %%eax\n"
		"movl %%eax,    %0\n"
		: "=r"(vAddr)
		:
		: "eax"
	);

	return vAddr;
}

void PageFault(u32 error)
{
	page_error_code_t* pError = (page_error_code_t*)(&error);

	printf("%s error : %p ", __FUNCTION__, error);

	if(IsEqual(pError->present, 0))
	{
		u32 vAddr = GetCr2();

		u32 pAddr = vAddr > (memoryBase + memorySize) ? (u32)(PMemAlloc(nullptr)) : vAddr;

		printf("vAddr : %p ==> pAddr : %p\n", vAddr, pAddr);

		LinkPage(vAddr, pAddr);
	}
}

void LinkPage(u32 vAddr, u32 pAddr)
{
	SetPageEntry(AddrOffset(GetPTE(vAddr, true), PTEIndex(vAddr)), PGEIndex(pAddr));

	FlushTLB(vAddr);
}

void UnLinkPage(u32 vAddr)
{
	page_entry_t* pte = GetPTE(vAddr, false);

	page_entry_t* entry = AddrOffset(pte, PTEIndex(vAddr));

	assert(entry->present);

	memset(entry, 0, sizeof(*entry));

	bool clear = true;

	for(int i = 0; !IsEqual(i, PAGE_MAX) && clear; ++i)
	{
		entry = (page_entry_t*)(AddrOffset(pte, i));

		clear = !IsEqual(entry->present, 1);
	}

	if(clear)
	{
		PMemFree(pte);

		page_entry_t* pde = GetPDE();

		entry = AddrOffset(pde, PDEIndex(vAddr));

		memset(entry, 0, sizeof(*entry));
	}
}

static page_entry_t* GetPDE()
{
	return (page_entry_t*)(0x3ff << 22 | 0x3ff << 12 | 0);
}

static page_entry_t* GetPTE(u32 vAddr, bool create)
{
	page_entry_t* ret = AddrOffset(GetPDE(), PDEIndex(vAddr));

	assert(create || (!create && ret->present));

	if(IsEqual(ret->present, 0))
	{
		page_entry_t* pte = (page_entry_t*)PMemAlloc(nullptr);

		memset(pte, 0, PAGE_SIZE);

		SetPageEntry(ret, PGEIndex(pte));
	}

	return (page_entry_t*)(ret->index << 12);
}