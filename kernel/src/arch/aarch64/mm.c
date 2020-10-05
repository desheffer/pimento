#include "asm/mm.h"
#include "critical.h"
#include "list.h"
#include "mm_context.h"
#include "page.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"

static va_table_t _kernel_l0[PAGE_SIZE] __attribute__((aligned (PAGE_SIZE))) = {0};
static va_table_t _kernel_l1[PAGE_SIZE] __attribute__((aligned (PAGE_SIZE))) = {0};

static unsigned _next_asid = 1;

/**
 * Assign a new ASID.
 */
static unsigned _assign_asid(void)
{
    critical_start();

    unsigned asid = _next_asid++;

    critical_end();

    return asid;
}

/**
 * Translate a physical address into a kernel virtual address.
 */
inline void * _paddr_to_vaddr(void * vaddr)
{
    return (void *) ((uintptr_t) vaddr | VA_START);
}

/**
 * Translate a kernel virtual address into a physical address.
 */
inline void * _vaddr_to_paddr(void * vaddr)
{
    return (void *) ((uintptr_t) vaddr & VA_MASK);
}

/**
 * Initialize the bare minimum to relocate the kernel.
 */
void mm_init(void)
{
    _kernel_l0[0] = (va_table_t) _vaddr_to_paddr(_kernel_l1)
        | PT_TABLE
        | PT_AF;

    for (unsigned i = 0; i < VA_TABLE_LENGTH; ++i) {
        _kernel_l1[i] = ((va_table_t) PAGE_SIZE * VA_TABLE_LENGTH * VA_TABLE_LENGTH * i)
            | PT_BLOCK
            | PT_AF
            | PT_nG
            | PT_ATTR(MT_DEVICE_nGnRnE);
    }

    uintptr_t asid = (uintptr_t) _assign_asid();
    uintptr_t ttbr1 = (asid << ASID_SHIFT) | (uintptr_t) _vaddr_to_paddr(_kernel_l0);
    uintptr_t ttbr0 = (asid << ASID_SHIFT) | (uintptr_t) _vaddr_to_paddr(_kernel_l0);

    // Set the translation table for kernel space.
    asm volatile("msr ttbr1_el1, %0" :: "r" (ttbr1));

    // Set the translation table for user space (temporary).
    asm volatile("msr ttbr0_el1, %0" :: "r" (ttbr0));
    asm volatile("isb");
}

/**
 * Set the memory break for the given context.
 */
void * mm_context_brk(struct mm_context * mm_context, void * addr)
{
    if (addr > mm_context->brk) {
        mm_context->brk = addr;
    }

    return mm_context->brk;
}

/**
 * Create a memory management context for a kernel task.
 */
struct mm_context * mm_context_create_kernel(void)
{
    struct mm_context * mm_context = kcalloc(sizeof(struct mm_context));

    // Initialize list of allocated pages.
    mm_context->pages = list_create();

    mm_context->pgd = _vaddr_to_paddr(_kernel_l0);

    return mm_context;
}

/**
 * Create a memory management context for a user task.
 */
struct mm_context * mm_context_create_user(void)
{
    struct mm_context * mm_context = kcalloc(sizeof(struct mm_context));

    // Initialize list of pages.
    mm_context->pages = list_create();
    mm_context->page_mappings = list_create();

    // Allocate page directory.
    struct page * page_dir = mm_context_page_alloc(mm_context);
    va_table_t * pgd = page_dir->vaddr;

    // Allocate kernel stack.
    struct page * page_kstack = mm_context_page_alloc(mm_context);
    mm_context->kernel_stack_init = (void *) ((uintptr_t) page_kstack->vaddr + PAGE_SIZE);

    mm_context->pgd = _vaddr_to_paddr(pgd);
    mm_context->asid = _assign_asid();
    mm_context->stack_init = (void *) STACK_INIT_USER;
    mm_context->brk = mm_context->stack_init;

    return mm_context;
}

struct mm_context * mm_context_create_user_clone(struct mm_context * old_mm_context)
{
    struct mm_context * mm_context = kcalloc(sizeof(struct mm_context));

    // Initialize list of allocated pages.
    mm_context->pages = list_create();
    mm_context->page_mappings = list_create();

    // Allocate page directory.
    struct page * page_dir = mm_context_page_alloc(mm_context);
    va_table_t * pgd = page_dir->vaddr;

    // Copy kernel stack.
    struct page * page_kstack = mm_context_page_alloc(mm_context);
    memcpy(page_kstack->vaddr, (void *) ((uintptr_t) old_mm_context->kernel_stack_init - PAGE_SIZE), PAGE_SIZE);
    mm_context->kernel_stack_init = (void *) ((uintptr_t) page_kstack->vaddr + PAGE_SIZE);

    mm_context->pgd = _vaddr_to_paddr(pgd);
    mm_context->asid = _assign_asid();
    mm_context->stack_init = old_mm_context->stack_init;
    mm_context->brk = old_mm_context->brk;

    // Copy page mappings.
    list_foreach(old_mm_context->page_mappings, struct page_mapping *, page_mapping) {
        critical_start();

        mm_copy_to_user(mm_context, page_mapping->vaddr, page_mapping->page->vaddr, PAGE_SIZE);

        critical_end();
    }

    return mm_context;
}

/**
 * Destroy a memory management context.
 */
void mm_context_destroy(struct mm_context * mm_context)
{
    struct page * page;

    // Deallocate pages that are no longer referenced.
    while ((page = list_pop_front(mm_context->pages))) {
        critical_start();

        if (--page->count == 0) {
            page_free(page);
        }

        critical_end();
    }

    list_destroy(mm_context->pages);

    struct page_mapping * page_mapping;

    // Remove page mappings.
    while ((page_mapping = list_pop_front(mm_context->page_mappings))) {
        kfree(page_mapping);
    }

    list_destroy(mm_context->page_mappings);

    kfree(mm_context);
}

/**
 * Get the kernel stack address for the given context.
 */
void * mm_context_kernel_stack_init(struct mm_context * mm_context)
{
    return mm_context->kernel_stack_init;
}

/**
 * Allocate an unmapped page for the given context.
 */
struct page * mm_context_page_alloc(struct mm_context * mm_context)
{
    struct page * page = page_alloc();

    // Zero out the contents of the page.
    memset(page->vaddr, 0, PAGE_SIZE);

    critical_start();

    list_push_back(mm_context->pages, page);

    critical_end();

    return page;
}

/**
 * Map a page for the given context.
 */
void mm_context_page_map(struct mm_context * mm_context, struct page * page,
                         void * vaddr)
{
    struct page_mapping * page_mapping = kcalloc(sizeof(struct page_mapping));

    page_mapping->page = page;
    page_mapping->vaddr = vaddr;

    critical_start();

    list_push_back(mm_context->page_mappings, page_mapping);

    critical_end();
}

/**
 * Get the stack address for the given context.
 */
void * mm_context_stack_init(struct mm_context * mm_context)
{
    return mm_context->stack_init;
}

/**
 * Calculate the index of `dest` for a lookup table of the given `level`.
 */
static unsigned _va_table_index(unsigned level, const void * dest)
{
    return ((uintptr_t) dest >> (39 - 9 * level)) & (VA_TABLE_LENGTH - 1);
}

/**
 * Given a lookup table and level, find an entry that corresponds to `dest`.
 */
static va_table_t * _va_table_traverse(va_table_t * parent, unsigned level,
                                       const void * dest)
{
    unsigned idx = _va_table_index(level, dest);
    uintptr_t mask = level < 3
        ? VA_TABLE_TABLE_ADDR_MASK
        : VA_TABLE_PAGE_ADDR_MASK;;

    parent = _paddr_to_vaddr(parent);

    return (va_table_t *) (parent[idx] & mask);
}

/**
 * Given a lookup table and level, add an entry that corresponds to `dest`.
 */
static va_table_t * _va_table_add(va_table_t * parent, unsigned level,
                                  const void * dest, struct page * page)
{
    unsigned idx = _va_table_index(level, dest);
    uintptr_t flags = level < 3
        ? (PT_TABLE | PT_AF)
        : (PT_PAGE | PT_AF | PT_USER | PT_ATTR(MT_NORMAL));

    parent = _paddr_to_vaddr(parent);
    va_table_t * child = _vaddr_to_paddr(page->vaddr);

    parent[idx] = (uintptr_t) child | flags;

    return child;
}

/**
 * Find the kernel virtual address that corresponds to a user virtual address.
 */
static void * _page_read_address(struct mm_context * mm_context,
                                 const void * dest)
{
    // Begin with the level 0 table.
    va_table_t * l0 = mm_context->pgd;

    // Traverse to level 1.
    va_table_t * l1 = _va_table_traverse(l0, 0, dest);
    if (l1 == 0) {
        return 0;
    }
    l1 = (va_table_t *) _paddr_to_vaddr(l1);

    // Traverse to level 2.
    va_table_t * l2 = _va_table_traverse(l1, 1, dest);
    if (l2 == 0) {
        return 0;
    }
    l2 = (va_table_t *) _paddr_to_vaddr(l2);

    // Traverse to level 3.
    va_table_t * l3 = _va_table_traverse(l2, 2, dest);
    if (l3 == 0) {
        return 0;
    }
    l3 = (va_table_t *) _paddr_to_vaddr(l3);

    // Traverse to the final page.
    uintptr_t page_start = (uintptr_t) _va_table_traverse(l3, 3, dest);
    if (page_start == 0) {
        return 0;
    }

    uintptr_t page_offset = (uintptr_t) dest & PAGE_OFFSET_MASK;

    return _paddr_to_vaddr((void *) (page_start | page_offset));
}

/**
 * Find the kernel virtual address that corresponds to a user virtual address.
 * If the given address is not mapped, then mapping tables will be created
 * along the way.
 */
static void * _page_write_address(struct mm_context * mm_context, void * dest)
{
    // Begin with the level 0 table.
    va_table_t * l0 = mm_context->pgd;

    // Traverse to level 1. Create a table if one does not exist.
    va_table_t * l1 = _va_table_traverse(l0, 0, dest);
    if (l1 == 0) {
        struct page * page = mm_context_page_alloc(mm_context);
        l1 = _va_table_add(l0, 0, dest, page);
    }
    l1 = (va_table_t *) _paddr_to_vaddr(l1);

    // Traverse to level 2. Create a table if one does not exist.
    va_table_t * l2 = _va_table_traverse(l1, 1, dest);
    if (l2 == 0) {
        struct page * page = mm_context_page_alloc(mm_context);
        l2 = _va_table_add(l1, 1, dest, page);
    }
    l2 = (va_table_t *) _paddr_to_vaddr(l2);

    // Traverse to level 3. Create a table if one does not exist.
    va_table_t * l3 = _va_table_traverse(l2, 2, dest);
    if (l3 == 0) {
        struct page * page = mm_context_page_alloc(mm_context);
        l3 = _va_table_add(l2, 2, dest, page);
    }
    l3 = (va_table_t *) _paddr_to_vaddr(l3);

    // Traverse to the final page. Create a page if one does not exist.
    uintptr_t page_start = (uintptr_t) _va_table_traverse(l3, 3, dest);
    if (page_start == 0) {
        struct page * page = mm_context_page_alloc(mm_context);
        page_start = (uintptr_t) _va_table_add(l3, 3, dest, page);

        // Record the mapping.
        void * dest_page_start = (void *) ((uintptr_t) dest & PAGE_START_MASK);
        mm_context_page_map(mm_context, page, dest_page_start);
    }

    uintptr_t page_offset = (uintptr_t) dest & PAGE_OFFSET_MASK;

    return _paddr_to_vaddr((void *) (page_start | page_offset));
}

/**
 * Copy from user memory space into kernel memory space. This function is
 * similar to `memcpy`, except that we have to maintain pointers to both the
 * kernel and user address spaces.
 */
size_t mm_copy_from_user(struct mm_context * mm_context, void * dest,
                         const void * src, size_t num)
{
    char * kdest = (char *) dest;
    const char * usrc = (const char *) src;
    char * ksrc = 0;
    size_t count = 0;

    while (num--) {
        // If we encounter a new page, then calculate the new page address.
        if (((uintptr_t) ksrc & PAGE_OFFSET_MASK) == 0) {
            ksrc = (char *) _page_read_address(mm_context, usrc);
            if (ksrc == 0) {
                break;
            }
        }

        *(kdest++) = *(ksrc++);
        ++usrc;
        ++count;
    }

    return count;
}

/**
 * Copy from kernel memory space into user memory space. This function is
 * similar to `memcpy`, except that we have to maintain pointers to both the
 * kernel and user address spaces.
 */
size_t mm_copy_to_user(struct mm_context * mm_context, void * dest,
                       const void * src, size_t num)
{
    char * udest = (char *) dest;
    const char * ksrc = (const char *) src;
    char * kdest = 0;
    size_t count = 0;

    while (num--) {
        // If we encounter a new page, then calculate the new page address.
        if (((uintptr_t) kdest & PAGE_OFFSET_MASK) == 0) {
            kdest = (char *) _page_write_address(mm_context, udest);
            if (ksrc == 0) {
                break;
            }
        }

        *(kdest++) = *(ksrc++);
        ++udest;
        ++count;
    }

    return count;
}

/**
 * Switch the current user memory space.
 */
void mm_switch_to(struct mm_context * mm_context)
{
    uintptr_t asid = (uintptr_t) mm_context->asid;
    uintptr_t ttbr1 = (asid << ASID_SHIFT) | (uintptr_t) _vaddr_to_paddr(_kernel_l0);
    uintptr_t ttbr0 = (asid << ASID_SHIFT) | (uintptr_t) mm_context->pgd;

    asm volatile("msr ttbr1_el1, %0" :: "r" (ttbr1));
    asm volatile("msr ttbr0_el1, %0" :: "r" (ttbr0));
    asm volatile("isb");
}

/**
 * Handle data aborts.
 */
void data_abort_handler(void * vaddr)
{
    struct task * task = scheduler_current_task();
    struct mm_context * mm_context = task->mm_context;

    _page_write_address(mm_context, vaddr);
}
