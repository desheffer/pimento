#include <serial.h>
#include <stdio.h>
#include <synchronize.h>

void kputc(void*, char c)
{
    Serial::putc(c);
}

void kputs(const char* s)
{
    enter_critical();

    while (*s) {
        kputc(0, *s);
        ++s;
    }

    leave_critical();
}

void kvprintf(const char* format, va_list arg)
{
    enter_critical();

    vcprintf(0, kputc, format, arg);

    leave_critical();
}

void kprintf(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    kvprintf(format, args);
    va_end(args);
}
