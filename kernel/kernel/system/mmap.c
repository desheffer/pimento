#include <assert.h>
#include <memory.h>
#include <mm.h>
#include <scheduler.h>
#include <synchronize.h>
#include <system.h>

void* sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    // @TODO
    (void) length;
    (void) prot;
    (void) flags;
    failif(fd != -1);
    failif(offset != 0);

    if (addr != 0) {
        return addr;
    }

    struct process* process = scheduler_current();

    enter_critical();

    long unsigned va_start = PAGE_SIZE;
    long unsigned va_end;

    // Locate an available range.
    while (1) {
        struct list_item* page_item = process->mm_context->pages->front;
        va_end = va_start + length;

        while (page_item != 0) {
            struct page* page = (struct page*) page_item->item;

            if (va_start <= (long unsigned) page->va && va_end >= (long unsigned) page->va) {
                va_start += PAGE_SIZE;
                break;
            }

            page_item = page_item->next;
        }

        if (page_item == 0) {
            break;
        }
    }

    // Allocate the pages.
    for (long unsigned va = va_start; va < va_end; va += PAGE_SIZE) {
        mm_map_page(process, (void*) va, alloc_page());
    }

    leave_critical();

    return (void*) va_start;
}
