#include <assert.h>
#include <kstdlib.h>
#include <limits.h>
#include <memory.h>
#include <mm.h>
#include <scheduler.h>
#include <string.h>
#include <synchronize.h>

static unsigned _next_asid = 1;

void mm_init(void)
{
    va_table_t * tables = (va_table_t *) kva_to_pa(&__va_table_start);

    memset(tables, 0, 4 * sizeof(va_table_t));

    // L0 row 0: [0xFFFF000000000000 - 0xFFFF007FFFFFFFFF]
    tables[0][0] = (long unsigned) &tables[1] |
        PT_TABLE |
        PT_AF;

    // L1 row 0: [0xFFFF000000000000 - 0xFFFF00003FFFFFFF]
    tables[1][0] = (long unsigned) &tables[2] |
        PT_TABLE |
        PT_AF;

    // L1 row 1: [0xFFFF000040000000 - 0xFFFF00007FFFFFFF]
    tables[1][1] = (long unsigned) 0x40000000 |
        PT_BLOCK |
        PT_AF |
        PT_XN |
        PT_ATTR(MT_DEVICE_nGnRnE);

    // L2 row 0: [0xFFFF000000000000 - 0xFFFF0000001FFFFF]
    tables[2][0] = (long unsigned) &tables[3] |
        PT_TABLE |
        PT_AF;

    // L2 rows 1 - 511: [0xFFFF000000200000 - 0xFFFF00003FFFFFFF]
    for (long unsigned i = 1; i < VA_TABLE_LENGTH; ++i) {
        long unsigned addr = i * PAGE_SIZE * VA_TABLE_LENGTH;
        long unsigned flags = 0;

        if (addr < 0x3F000000) {
            flags |= PT_ATTR(MT_NORMAL);
        } else {
            flags |= PT_ATTR(MT_DEVICE_nGnRnE);
        }

        tables[2][i] = addr |
            PT_BLOCK |
            flags |
            PT_AF |
            PT_XN;
    }

    // L3 rows 0 - 511: [0xFFFF000000000000 - 0xFFFF0000001FFFFF]
    // This block assumes a 2M limit on the text and rodata sections.
    for (long unsigned i = 0; i < VA_TABLE_LENGTH; ++i) {
        long unsigned addr = i * PAGE_SIZE;
        long unsigned flags = 0;

        if (addr >= (long unsigned) kva_to_pa(&__text_start)
            && addr < (long unsigned) kva_to_pa(&__text_end)
        ) {
            flags |= PT_RO;
        } else if (addr >= (long unsigned) kva_to_pa(&__rodata_start)
            && addr < (long unsigned) kva_to_pa(&__rodata_end)
        ) {
            flags |= PT_RO | PT_PXN;
        } else {
            flags |= PT_PXN;
        }

        tables[3][i] = addr |
            PT_PAGE |
            flags |
            PT_AF |
            PT_ATTR(MT_NORMAL);
    }

    // Set the translation table for user space (temporary).
    asm volatile("msr ttbr0_el1, %0" :: "r" (tables));

    // Set the translation table for kernel space.
    asm volatile("msr ttbr1_el1, %0" :: "r" (tables));
}

static void record_alloc(struct process * process, void * pa, void * va, unsigned flags)
{
    struct page * page = kzalloc(sizeof(struct page));
    page->pa = pa;
    page->va = va;
    page->flags = flags;

    list_push_back(process->mm_context->pages, page);
}

void mm_copy_from(struct process * parent, struct process * child)
{
    struct list_item * page_item = parent->mm_context->pages->front;

    while (page_item != 0) {
        struct page * page = (struct page *) page_item->item;

        if ((page->flags & PG_VM) != 0) {
            assert(page->va != 0);

            void * child_pa = alloc_page();

            mm_map_page(child, page->va, child_pa);

            memcpy(pa_to_kva(child_pa), pa_to_kva(page->pa), PAGE_SIZE);
        }

        page_item = page_item->next;
    }
}

static unsigned assign_asid(void)
{
    enter_critical();

    unsigned asid = _next_asid++;
    failif(asid > USHRT_MAX);

    leave_critical();

    return asid;
}

void mm_create_kstack(struct process * process)
{
    // @TODO: Don't allow PT_USER access.
    for (unsigned i = KSTACK_SIZE; i > 0; --i) {
        mm_map_page(process, (void *) ((long unsigned) KSTACK_TOP - i * PAGE_SIZE), alloc_page());
    }
}

static unsigned va_table_index(void * va, unsigned level)
{
    assert(level < 4);

    return ((long unsigned) va >> (39 - 9 * level)) & 0x1FF;
}

#define va_table_access(pa) (*((va_table_t *) pa_to_kva(pa)))

static va_table_t * add_table(struct process * process, va_table_t * tab, void * va, unsigned level)
{
    unsigned index = va_table_index(va, level);

    if (!(va_table_access(tab)[index] & PT_TABLE)) {
        va_table_t * new_tab = alloc_page();

        record_alloc(process, (void *) new_tab, 0, 0);

        va_table_access(tab)[index] = (long unsigned) new_tab |
            PT_TABLE |
            PT_AF;
    }

    long unsigned row = va_table_access(tab)[index];
    return (va_table_t *) (row & VA_TABLE_TABLE_ADDR_MASK);
}

static void * add_page(struct process * process, va_table_t * tab, void * va, void * pa)
{
    unsigned index = va_table_index(va, 3);

    record_alloc(process, pa, va, PG_USER | PG_VM);

    va_table_access(tab)[index] = (long unsigned) pa |
        PT_PAGE |
        PT_AF |
        PT_NG |
        PT_USER |
        PT_ATTR(MT_NORMAL);

    return pa;
}

static void * page_addr(void * addr)
{
    return (void *) ((long unsigned) addr & PAGE_MASK);
}

void mm_map_page(struct process * process, void * va, void * pa)
{
    va = page_addr(va);

    va_table_t * tab = (va_table_t *) process->mm_context->pgd;

    for (unsigned level = 0; level < 3; ++level) {
        tab = add_table(process, tab, va, level);
    }

    add_page(process, tab, va, kva_to_pa(pa));
}

void mm_process_create(struct process * process)
{
    process->mm_context = kzalloc(sizeof(struct mm_context));

    // Initialize list of allocated pages.
    process->mm_context->pages = list_create();

    // Allocate page global directory.
    process->mm_context->asid = assign_asid();
    process->mm_context->pgd = alloc_page();

    record_alloc(process, (void *) process->mm_context->pgd, 0, 0);

    process->mm_context->brk = (void *) BRK_START;
}

void mm_process_destroy(struct process * process)
{
    kfree(process->mm_context);
}

void mm_switch_to(struct process * process)
{
    long unsigned asid = (long unsigned) process->mm_context->asid;
    long unsigned pgd = (long unsigned) process->mm_context->pgd;

    ttbr_switch_to((asid << 48) | pgd);
}

void data_abort_handler(void * va)
{
    struct process * process = scheduler_current();

    mm_map_page(process, va, alloc_page());
}
