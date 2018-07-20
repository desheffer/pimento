#pragma once

#include <stddef.h>

void* malloc(size_t);
void free(void*);

void* operator new(size_t, void*);
