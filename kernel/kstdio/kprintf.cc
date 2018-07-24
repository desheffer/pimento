#include <serial.h>
#include <stdio.h>
#include <synchronize.h>

static void kputc(void*, char c)
{
    Serial::putc(c);
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
