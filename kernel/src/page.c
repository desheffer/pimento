#include "critical.h"
#include "page.h"
#include "pimento.h"

static unsigned _page_alloc_next = 0;
static unsigned _page_count = 0;
static size_t _page_size;
static struct page * _pages = 0;

/**
 * Initialize page allocation given the desired range of memory and page size.
 * The allocation table will be placed at the beginning of this range since we
 * don't have the luxury of using `kmalloc()` during this process.
 */
void page_init(void * start, void * end, size_t page_size)
{
    _page_count = ((char *) end - (char *) start) / page_size;
    _page_size = page_size;
    _pages = start;

    char * addr = start;
    char * first_free = (char *) &_pages[_page_count];

    for (unsigned index = 0; index < _page_count; ++index) {
        _pages[index].vaddr = addr;
        _pages[index].count = addr < first_free ? 1 : 0;

        addr += page_size;
    }
}

/**
 * Allocate a page.
 */
struct page * page_alloc(void)
{
    struct page * page = 0;
    unsigned index = _page_alloc_next;

    while (page == 0) {
        if (index >= _page_count) {
            index = 0;
        }

        if (_pages[index].count == 0) {
            critical_start();

            if (_pages[index].count == 0) {
                page = &_pages[index];
                page->count = 1;
            }

            critical_end();
        }

        ++index;
    }

    _page_alloc_next = index;

    return page;
}

/**
 * Get the count of pages being managed.
 */
unsigned page_count(void)
{
    return _page_count;
}

/**
 * Free a page.
 */
void page_free(struct page * page)
{
    critical_start();

    --page->count;

    critical_end();
}

/**
 * Get the array of pages.
 */
struct page * page_pages(void)
{
    return _pages;
}

/**
 * Get the size of one page in bytes.
 */
size_t page_size(void)
{
    return _page_size;
}
