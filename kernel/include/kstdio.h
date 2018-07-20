#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void init_kprintf(void*, putc_t);

void kputs(const char*);

void kvprintf(const char*, va_list);

void kprintf(const char*, ...) __attribute__((format(printf, 1, 2)));
