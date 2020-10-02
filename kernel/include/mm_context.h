#pragma once

#include "pimento.h"

struct mm_context;

void * mm_context_brk(struct mm_context *, void *);
struct mm_context * mm_context_create_kernel(void);
struct mm_context * mm_context_create_user(void);
struct mm_context * mm_context_create_user_clone(struct mm_context *);
void mm_context_destroy(struct mm_context *);
void * mm_context_kernel_stack_init(struct mm_context *);
struct page * mm_context_page_alloc(struct mm_context *);
void mm_context_page_map(struct mm_context *, struct page *, void *);
void * mm_context_stack_init(struct mm_context *);
size_t mm_copy_from_user(struct mm_context *, void *, const void *, size_t);
size_t mm_copy_to_user(struct mm_context *, void *, const void *, size_t);
