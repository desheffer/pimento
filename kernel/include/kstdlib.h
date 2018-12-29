#pragma once

#include <stddef.h>

void kfree(void*);
void* kmalloc(size_t);
void* kzalloc(size_t);
