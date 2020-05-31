#pragma once

#include <stddef.h>

void * kmalloc(size_t);
void * kcalloc(size_t);
void kfree(void *);
