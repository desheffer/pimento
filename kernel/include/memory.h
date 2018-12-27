#pragma once

#include <mmap.h>
#include <process.h>

typedef struct page_t {
    unsigned allocated: 1;
} page_t;

void memory_init();
void memory_reserve_range(void*, void*);

void* alloc_kernel_page();
void* alloc_user_page(process_t*);

void free_kernel_page(void*);
void free_user_page(process_t*, void*);

extern char __start;
extern char __text_start;
extern char __text_end;
extern char __rodata_start;
extern char __rodata_end;
extern char __data_start;
extern char __data_end;
extern char __va_table_start;
extern char __va_table_end;
extern char __bss_start;
extern char __bss_end;
extern char __end;
