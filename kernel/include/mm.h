#pragma once

#define STACK_ALIGN(addr) (((addr) + 15) & ~15)

#define PAGE_SIZE 4096
#define PAGE_MASK (0xFFFFFFFFFFFFFFFF - (PAGE_SIZE - 1))

#define VA_BITS  48
#define VA_START (0xFFFFFFFFFFFFFFFF - ((0x1UL << VA_BITS) - 1))

#define VA_TABLE_LENGTH 512

#define VA_TABLE_TABLE_ADDR_MASK 0xFFFFFFFFF000
#define VA_TABLE_PAGE_ADDR_MASK  0xFFFFFFFFF000

#define TCR_ASID16     (0b1UL  << 36) // Use 16 bits of ASID
#define TCR_TG1_4KB    (0b10UL << 30) // 4KB granule size
#define TCR_TG0_4KB    (0b00UL << 14) // 4KB granule size
#define TCR_TxSZ(bits) (((64 - bits) << 16) | ((64 - bits) << 0))

#define TCR_EL1 \
    ( \
        TCR_TxSZ(VA_BITS) | \
        TCR_ASID16 | TCR_TG1_4KB | TCR_TG0_4KB \
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
#define PT_NG    (0b1UL  << 11) // Not global
#define PT_PXN   (0b1UL  << 53) // Privileged execute-never
#define PT_XN    (0b1UL  << 54) // Execute-never

#define PT_ATTR(n) ((n) << 2)

#define pa_to_kva(ptr) ((void *) ((long unsigned) (ptr) | VA_START))
#define kva_to_pa(ptr) ((void *) ((long unsigned) (ptr) & ~VA_START))

#define PG_VM   1
#define PG_USER 2

#define KSTACK_TOP  0x40000000
#define KSTACK_SIZE 2
#define USTACK_TOP  (KSTACK_TOP - KSTACK_SIZE * PAGE_SIZE)

#ifndef __ASSEMBLY__

#include <process.h>

typedef long unsigned va_table_t[VA_TABLE_LENGTH];

typedef long unsigned * pgd_t;

struct page {
    void * pa;
    void * va;
    unsigned flags;
};

struct mm_context {
    pgd_t pgd;
    unsigned asid;
    struct list * pages;
    void * brk;
};

void mm_init(void);
void mm_copy_from(struct process *, struct process *);
void mm_create_kstack(struct process *);
void mm_map_page(struct process *, void *, void *);
void mm_process_create(struct process *);
void mm_process_destroy(struct process *);
void mm_switch_to(struct process *);

void data_abort_handler(void *);
void ttbr_switch_to(long unsigned);

#endif
