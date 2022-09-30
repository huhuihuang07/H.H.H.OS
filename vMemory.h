#ifndef VMEMORY_H
#define VMEMORY_H

#include "pMemory.h"

#ifndef PGEIndex
#define PGEIndex(addr) \
		(((u32)(addr)) >> 12u)
#endif

#ifndef PDEIndex
#define PDEIndex(addr) \
		((((u32)(addr)) >> 22u) & 0x3ff)
#endif 		

#ifndef PTEIndex
#define PTEIndex(addr) \
		((((u32)(addr)) >> 12u) & 0x3ff)
#endif

#ifndef Index 
#define Index(i, j) \
        ((i) * 1024 + (j))
#endif		

#ifndef PAGE_MAX
#define PAGE_MAX \
        (PAGE_SIZE / sizeof(page_entry_t))
#endif       

#ifndef PDE_SIZE
#define PDE_SIZE (PAGE_SIZE * PAGE_MAX)
#endif 

typedef struct 
{
    u8 present  : 1; // 在内存中
    u8 write    : 1; // 0 只读 1 可读可写
    u8 user     : 1; // 1 所有人 0 超级用户 DPL < 3
    u8 pwt      : 1; // page write through 1 直写模式，0 回写模式
    u8 pcd      : 1; // page cache disable 禁止该页缓冲
    u8 accessed : 1; // 被访问过，用于统计使用频率
    u8 dirty    : 1; // 脏页，表示该页缓冲被写过
    u8 pat      : 1; // page attribute table 页大小 4K/4M
    u8 global   : 1; // 全局，所有进程都用到了，该页不刷新缓冲
    u8 ignored  : 3; // 该安排的都安排了，送给操作系统吧
    u32 index   : 20;// 页索引
} _packed page_entry_t;

typedef struct 
{
    u8 present    : 1;
    u8 write      : 1;
    u8 user       : 1;
    u8 reserved0  : 1;
    u8 fetch      : 1;
    u8 protection : 1;
    u8 shadow     : 1;
    u16 reserved1 : 8;
    u8 sgx        : 1;
    u16 reserved2;
} _packed page_error_code_t;

void vMemoryModuleInit();

void PageFault(u32 error);

void LinkPage(u32 vAddr, u32 pAddr);
void UnLinkPage(u32 vAddr);
void* CreatePDE();

static page_entry_t* GetPDE();
static page_entry_t* GetPTE(u32 vAddr, bool create);

static void EnablePage();
static void DisablePage();

static u32 GetCr3();
static void SetCr3(u32 pde);

static u32 GetCr2();

static void FlushTLB(u32 vaddr);

static void SetPageEntry(page_entry_t* entry, u32 index);

extern void PageFaultInit();

extern const u32 gMemSize;

#endif //!VMEMORY_H