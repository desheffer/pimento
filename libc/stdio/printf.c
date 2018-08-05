#include <stdio.h>
#include <unistd.h>

static void vprintf_putc(void* data __attribute__((unused)), const char c)
{
    write(STDOUT_FILENO, &c, 1);
}

void vprintf(const char* format, va_list arg)
{
    vcprintf(0, vprintf_putc, format, arg);
}

void printf(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
