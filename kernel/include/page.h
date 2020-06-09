#pragma once

#include <stddef.h>

struct page {
    void * vaddr;
    unsigned count;
};

void page_init(void *, void *, size_t);
struct page * page_alloc(void);
unsigned page_count(void);
void page_free(struct page *);
struct page * page_pages(void);
size_t page_size(void);
