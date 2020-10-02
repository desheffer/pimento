#pragma once

#include "pimento.h"

typedef void (* putc_t)(void *, char);

void vcprintf(void *, putc_t, const char *, va_list);

void kprintf(const char *, ...) __attribute__((format(printf, 1, 2)));
void kputc(void *, const char);
void kputs(const char *);
void set_kputc(putc_t);
