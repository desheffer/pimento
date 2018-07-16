#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

typedef void (*putc_t)(void*, char);

void init_printf(void*, putc_t);

void puts(const char*);

void vcprintf(void*, putc_t, const char*, va_list);
void vprintf(const char*, va_list);
size_t vsnprintf(char*, size_t, const char*, va_list);
size_t vsprintf(char*, const char*, va_list);

void cprintf(void*, putc_t, const char*, ...) __attribute__((format(printf, 3, 4)));
void printf(const char*, ...) __attribute__((format(printf, 1, 2)));
size_t snprintf(char*, size_t, const char*, ...) __attribute__((format(printf, 3, 4)));
size_t sprintf(char*, const char*, ...) __attribute__((format(printf, 2, 3)));
