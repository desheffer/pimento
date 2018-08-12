#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

void kprintf(const char*, ...) __attribute__((format(printf, 1, 2)));
void kputs(const char*);
void kvprintf(const char*, va_list);
