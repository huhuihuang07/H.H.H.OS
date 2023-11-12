#pragma once

#include "pMemory.h"

#ifndef PGEIndex
#define PGEIndex(addr) \
    (((uint32_t)(addr)) >> 12u)
#endif

#ifndef PDEIndex
#define PDEIndex(addr) \
    ((((uint32_t)(addr)) >> 22u) & 0x3ff)
#endif

#ifndef PTEIndex
#define PTEIndex(addr) \
    ((((uint32_t)(addr)) >> 12u) & 0x3ff)
#endif

#ifndef Index
#define Index(i, j) \
    ((((uint32_t)(i)) << 10u) + (j))
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
    uint8_t present  : 1;  // 在内存中
    uint8_t write    : 1;  // 0 只读 1 可读可写
    uint8_t user     : 1;  // 1 所有人 0 超级用户 DPL < 3
    uint8_t pwt      : 1;  // page write through 1 直写模式，0 回写模式
    uint8_t pcd      : 1;  // page cache disable 禁止该页缓冲
    uint8_t accessed : 1;  // 被访问过，用于统计使用频率
    uint8_t dirty    : 1;  // 脏页，表示该页缓冲被写过
    uint8_t pat      : 1;  // page attribute table 页大小 4K/4M
    uint8_t global   : 1;  // 全局，所有进程都用到了，该页不刷新缓冲
    uint8_t ignored  : 3;  // 该安排的都安排了，送给操作系统吧
    uint32_t index   : 20; // 页索引
} _packed page_entry_t;

typedef struct
{
    uint8_t present    : 1;
    uint8_t write      : 1;
    uint8_t user       : 1;
    uint8_t reserved0  : 1;
    uint8_t fetch      : 1;
    uint8_t protection : 1;
    uint8_t shadow     : 1;
    uint16_t reserved1 : 8;
    uint8_t sgx        : 1;
    uint16_t reserved2;
} _packed page_error_code_t;

void vMemoryModuleInit();

void PageFault(uint32_t error);

void LinkPage(uint32_t vAddr, uint32_t pAddr);
void UnLinkPage(uint32_t vAddr);
void* CreatePDE();

extern void PageFaultInit();

extern const uint32_t gMemSize;