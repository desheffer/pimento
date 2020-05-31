#pragma once

#include <stddef.h>

struct page {
    void * addr;
    unsigned count;
};

void page_init(void *, void *, size_t);
struct page * page_alloc(void);
void page_free(struct page *);
size_t page_size(void);
