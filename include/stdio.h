#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#ifndef STDIO_H
#define STDIO_H

typedef void (*putc_t)(void*, char);

void init_printf(void*, putc_t);

void vcprintf(void*, putc_t, const char*, va_list);
void vprintf(const char*, va_list);
uint32_t vsnprintf(char*, size_t, const char*, va_list);
uint32_t vsprintf(char*, const char*, va_list);

void cprintf(void*, putc_t, const char*, ...);
void printf(const char*, ...);
uint32_t snprintf(char*, size_t, const char*, ...);
uint32_t sprintf(char*, const char*, ...);

#endif
