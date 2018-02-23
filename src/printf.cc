#include <stdio.h>
#include <string.h>

#define BUF_MAX 13

static putc_t vprintf_putc = 0;
static void* vprintf_putc_data = 0;

void init_printf(void* data, putc_t putc)
{
    vprintf_putc_data = data;
    vprintf_putc = putc;
}

struct vcprintf_specifiers_t {
    char* buf;
    bool alt;
    bool leftAlign;
    bool uppercase;
    char sign;
    unsigned base;
    unsigned pad;
    unsigned width;
};

static void vcprintf_char(vcprintf_specifiers_t* sp, char c)
{
    *sp->buf = c;
    *(sp->buf + 1) = 0;
}

static void vcprintf_int(vcprintf_specifiers_t* sp, int i)
{
    unsigned digit;

    if (i < 0) {
        sp->sign = '-';
        i = -i;
    }

    sp->buf += BUF_MAX - 1;
    *sp->buf = 0;

    do {
        digit = i % sp->base;
        *(--sp->buf) = digit + (digit > 9 ? (sp->uppercase ? 'A' : 'a') - 10 : '0');
        i = i / sp->base;
    } while (i);
}

static void vcprintf_uint(vcprintf_specifiers_t* sp, unsigned i)
{
    unsigned digit;

    sp->buf += BUF_MAX - 1;
    *sp->buf = 0;

    do {
        digit = i % sp->base;
        *(--sp->buf) = digit + (digit > 9 ? (sp->uppercase ? 'A' : 'a') - 10 : '0');
        i = i / sp->base;
    } while (i);
}

static void vcprintf_sp(void* data, putc_t putc, vcprintf_specifiers_t* sp)
{
    const char* buf = sp->buf;
    size_t len;

    len = strlen(sp->buf);

    if (sp->sign) {
        len++;
    }

    if (sp->alt && (sp->base == 8 || sp->base == 16)) {
        putc(data, '0');
        len++;
    }

    if (sp->alt && sp->base == 16) {
        putc(data, sp->uppercase ? 'X' : 'x');
        len++;
    }

    if (sp->sign && (sp->pad || sp->leftAlign)) {
        putc(data, sp->sign);
    }

    if (!sp->leftAlign) {
        while (len < sp->width) {
            putc(data, sp->pad ? '0' : ' ');
            len++;
        }
    }

    if (sp->sign && !(sp->pad || sp->leftAlign)) {
        putc(data, sp->sign);
    }

    while (*buf) {
        putc(data, *buf);
        buf++;
    }

    if (sp->leftAlign) {
        while (len < sp->width) {
            putc(data, ' ');
            len++;
        }
    }
}

void vcprintf(void* data, putc_t putc, const char* format, va_list arg)
{
    vcprintf_specifiers_t sp;
    char buf[BUF_MAX];

    while (*format) {
        if (*format != '%') {
            putc(data, *format);
            format++;
        } else {
            format++;

            sp.buf = buf;
            sp.alt = false;
            sp.base = 0;
            sp.leftAlign = false;
            sp.pad = 0;
            sp.sign = 0;
            sp.uppercase = false;
            sp.width = 0;

            // Flags
            while (*format) {
                switch (*format) {
                    case '-':
                        sp.leftAlign = true;
                        sp.pad = 0;
                        format++;
                        continue;
                    case '+':
                        sp.sign = '+';
                        format++;
                        continue;
                    case '0':
                        sp.pad = 1;
                        sp.leftAlign = false;
                        format++;
                        continue;
                    case '#':
                        sp.alt = true;
                        format++;
                        continue;
                }
                break;
            }

            // Width
            while (*format) {
                if (*format >= '0' && *format <= '9') {
                    sp.width = (sp.width * 10) + (*format - '0');
                    format++;
                    continue;
                } else if (*format == '*') {
                    sp.width = va_arg(arg, unsigned);
                    format++;
                }
                break;
            }

            // Precision
            if (*format == '.') {
                format++;

                while (*format) {
                    if (*format >= '0' && *format <= '9') {
                        format++;
                        continue;
                    }
                    break;
                }
            }

            // Specifier
            if (*format) {
                switch (*format) {
                    case 'c':
                        // Character
                        vcprintf_char(&sp, (char) va_arg(arg, unsigned));
                        break;
                    case 'd':
                    case 'i':
                        // Signed decimal integer
                        sp.base = 10;
                        vcprintf_int(&sp, va_arg(arg, int));
                        break;
                    case 'o':
                        // Unsigned octal
                        sp.base = 8;
                        vcprintf_uint(&sp, va_arg(arg, unsigned));
                        break;
                    case 's':
                        // String of characters
                        sp.buf = va_arg(arg, char*);
                        break;
                    case 'u':
                        // Unsigned decimal integer
                        sp.base = 10;
                        vcprintf_uint(&sp, va_arg(arg, unsigned));
                        break;
                    case 'x':
                    case 'p':
                        // Unsigned hexadecimal uppercase
                        sp.base = 16;
                        vcprintf_uint(&sp, va_arg(arg, unsigned));
                        break;
                    case 'X':
                        // Signed hexadecimal uppercase
                        sp.base = 16;
                        sp.uppercase = true;
                        vcprintf_uint(&sp, va_arg(arg, unsigned));
                        break;
                    case '%':
                        // Literal
                        putc(data, *format);
                        format++;
                        continue;
                    default:
                        format++;
                        continue;
                }
                format++;

                vcprintf_sp(data, putc, &sp);
            }
        }
    }
}

void vprintf(const char* format, va_list arg)
{
    if (vprintf_putc) {
        vcprintf(vprintf_putc_data, vprintf_putc, format, arg);
    }
}

struct vsnprintf_putc_data_t {
    char* dest;
    size_t numChars;
    size_t maxChars;
};

static void vsnprintf_putc(void* data, char c)
{
    vsnprintf_putc_data_t* putc_data = (vsnprintf_putc_data_t*) data;

    if (putc_data->numChars < putc_data->maxChars) {
        putc_data->dest[putc_data->numChars++] = c;
    }
}

size_t vsnprintf(char* s, size_t max, const char* format, va_list arg)
{
    vsnprintf_putc_data_t putc_data;
    putc_data.dest = s;
    putc_data.numChars = 0;
    putc_data.maxChars = max;

    vcprintf(&putc_data, &vsnprintf_putc, format, arg);

    putc_data.dest[putc_data.numChars] = 0;

    return putc_data.numChars;
}

size_t vsprintf(char* s, const char* format, va_list arg)
{
    return vsnprintf(s, UINT32_MAX, format, arg);
}

void cprintf(void* data, putc_t putc, const char* format, ...)
{
    va_list args;

    va_start(args, format);
    vcprintf(data, putc, format, args);
    va_end(args);
}

void printf(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

size_t snprintf(char* s, size_t size, const char* format, ...)
{
    va_list args;
    size_t retval;

    va_start(args, format);
    retval = vsnprintf(s, size, format, args);
    va_end(args);

    return retval;
}

size_t sprintf(char* s, const char* format, ...)
{
    va_list args;
    size_t retval;

    va_start(args, format);
    retval = vsprintf(s, format, args);
    va_end(args);

    return retval;
}
