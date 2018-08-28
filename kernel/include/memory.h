#pragma once

#define PAGE_SIZE 4096

#define VA_BITS  48
#define VA_START (0xFFFFFFFFFFFFFFFF - ((0x1UL << VA_BITS) - 1))

#define VA_TABLE_LENGTH 512

#define TCR_EL1 \
    ( \
        /* TG1 = 4KB granule size */ \
        (0b10UL << 30) | \
        /* T1SZ = Size offset of memory region */ \
        ((64 - VA_BITS) << 16) | \
        /* TG0 = 4KB granule size */ \
        (0b00UL << 14) | \
        /* T0SZ = Size offset of memory region */ \
        ((64 - VA_BITS) << 0) \
    )

#define MT_DEVICE_nGnRnE 0
#define MT_NORMAL        1
#define MT_NORMAL_NC     2

#define MAIR_EL1 \
    ( \
        (0x00UL << (8 * MT_DEVICE_nGnRnE)) | \
        (0xFFUL << (8 * MT_NORMAL)) | \
        (0x44UL << (8 * MT_NORMAL_NC)) \
    )

#define PT_BLOCK (0b01UL <<  0) // Block descriptor
#define PT_TABLE (0b11UL <<  0) // Table descriptor
#define PT_PAGE  (0b11UL <<  0) // Page descriptor
#define PT_USER  (0b1UL  <<  6) // Unprivileged
#define PT_RO    (0b1UL  <<  7) // Read-Only
#define PT_OSH   (0b10UL <<  8) // Outer Shareable
#define PT_ISH   (0b11UL <<  8) // Inner Shareable
#define PT_AF    (0b1UL  << 10) // Access flag
#define PT_XN    (0b11UL << 53) // Execute-never

#define PT_ATTR(n) ((n) << 2)

#ifndef __ASSEMBLY__

#include <stddef.h>

typedef long unsigned va_table[VA_TABLE_LENGTH];

typedef struct {
    unsigned allocated: 1;
} page_t;

void memory_init_kernel_table();
void memory_init();
void memory_reserve_range(void*, void*);

void* alloc_page();
void free_page(void*);

extern char __start;
extern char __text_start;
extern char __text_end;
extern char __rodata_start;
extern char __rodata_end;
extern char __data_start;
extern char __data_end;
extern char __bss_start;
extern char __bss_end;
extern char __end;

#endif
