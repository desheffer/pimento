#pragma once

#include <pimento.h>

struct mm_context;

void * mm_context_brk(struct mm_context *, void *);
struct mm_context * mm_context_create_kernel(void);
struct mm_context * mm_context_create_user(void);
void mm_context_destroy(struct mm_context *);
struct page * mm_context_page_alloc(struct mm_context *);
void * mm_context_stack_top(struct mm_context *);
size_t mm_copy_from_user(struct mm_context *, void *, const void *, size_t);
size_t mm_copy_to_user(struct mm_context *, void *, const void *, size_t);
