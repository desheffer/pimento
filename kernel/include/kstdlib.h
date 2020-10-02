#pragma once

#include "pimento.h"

void * kmalloc(size_t);
void * kcalloc(size_t);
void kfree(void *);
unsigned long strtoul(const char *, char **, int);
