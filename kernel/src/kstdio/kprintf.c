#include <critical.h>
#include <pimento.h>

static putc_t _kputc = 0;

void kprintf(const char * format, ...)
{
    va_list args;

    va_start(args, format);

    critical_start();

    vcprintf(0, kputc, format, args);

    critical_end();

    va_end(args);
}

void kputc(void * data, const char c)
{
    if (_kputc != 0) {
        _kputc(data, c);
    }
}

void kputs(const char * s)
{
    while (*s) {
        kputc(0, *(s++));
    }
}

void set_kputc(putc_t new_kputc)
{
    _kputc = new_kputc;
}
