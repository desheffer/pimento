#pragma once

#include <stddef.h>

#define PAGE_SIZE 4096

extern void* __heap_start;

typedef struct {
    unsigned allocated: 1;
} page_t;

void memory_init();
unsigned memory_page_count();
size_t memory_page_size();
void memory_reserve_range(void*, void*);

void* alloc_page();
void free_page(void*);
