#include <assert.h>
#include <kstdio.h>
#include <kstdlib.h>
#include <string.h>

#define BUF_MAX 13

struct vcprintf_specifiers {
    char* buf;
    unsigned alt;
    unsigned base;
    unsigned left_align;
    unsigned pad;
    unsigned uppercase;
    unsigned width;
    char sign;
};

static void vcprintf_char(struct vcprintf_specifiers* sp, char c)
{
    *sp->buf = c;
    *(sp->buf + 1) = 0;
}

static void vcprintf_int(struct vcprintf_specifiers* sp, int i)
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

static void vcprintf_uint(struct vcprintf_specifiers* sp, unsigned i)
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

static void vcprintf_sp(void* data, putc_t putc_f, struct vcprintf_specifiers* sp)
{
    const char* buf = sp->buf;
    size_t len;

    len = strlen(sp->buf);

    if (sp->sign) {
        ++len;
    }

    if (sp->alt && (sp->base == 8 || sp->base == 16)) {
        putc_f(data, '0');
        ++len;
    }

    if (sp->alt && sp->base == 16) {
        putc_f(data, sp->uppercase ? 'X' : 'x');
        ++len;
    }

    if (sp->sign && (sp->pad || sp->left_align)) {
        putc_f(data, sp->sign);
    }

    if (!sp->left_align) {
        while (len < sp->width) {
            putc_f(data, sp->pad ? '0' : ' ');
            ++len;
        }
    }

    if (sp->sign && !(sp->pad || sp->left_align)) {
        putc_f(data, sp->sign);
    }

    while (*buf) {
        putc_f(data, *buf);
        ++buf;
    }

    if (sp->left_align) {
        while (len < sp->width) {
            putc_f(data, ' ');
            ++len;
        }
    }
}

void vcprintf(void* data, putc_t putc_f, const char* format, va_list arg)
{
    struct vcprintf_specifiers* sp = (struct vcprintf_specifiers*) kzalloc(sizeof(struct vcprintf_specifiers));

    char* buf = (char*) kzalloc(BUF_MAX);

    while (*format) {
        if (*format != '%') {
            putc_f(data, *format);
            ++format;
        } else {
            ++format;

            sp->buf = buf;
            sp->alt = 0;
            sp->base = 0;
            sp->left_align = 0;
            sp->pad = 0;
            sp->sign = 0;
            sp->uppercase = 0;
            sp->width = 0;

            // Flags
            while (*format) {
                switch (*format) {
                    case '-':
                        sp->left_align = 1;
                        sp->pad = 0;
                        ++format;
                        continue;
                    case '+':
                        sp->sign = '+';
                        ++format;
                        continue;
                    case '0':
                        sp->pad = 1;
                        sp->left_align = 0;
                        ++format;
                        continue;
                    case '#':
                        sp->alt = 1;
                        ++format;
                        continue;
                }
                break;
            }

            // Width
            while (*format) {
                if (*format >= '0' && *format <= '9') {
                    sp->width = (sp->width * 10) + (*format - '0');
                    ++format;
                    continue;
                } else if (*format == '*') {
                    sp->width = va_arg(arg, unsigned);
                    ++format;
                }
                break;
            }

            // Precision
            if (*format == '.') {
                ++format;

                while (*format) {
                    if (*format >= '0' && *format <= '9') {
                        ++format;
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
                        vcprintf_char(sp, (char) va_arg(arg, int));
                        break;
                    case 'd':
                    case 'i':
                        // Signed decimal integer
                        sp->base = 10;
                        vcprintf_int(sp, va_arg(arg, int));
                        break;
                    case 'o':
                        // Unsigned octal
                        sp->base = 8;
                        vcprintf_uint(sp, va_arg(arg, unsigned));
                        break;
                    case 's':
                        // String of characters
                        sp->buf = va_arg(arg, char*);
                        break;
                    case 'u':
                        // Unsigned decimal integer
                        sp->base = 10;
                        vcprintf_uint(sp, va_arg(arg, unsigned));
                        break;
                    case 'x':
                        // Unsigned hexadecimal lowercase
                        sp->base = 16;
                        vcprintf_uint(sp, va_arg(arg, unsigned));
                        break;
                    case 'X':
                        // Unsigned hexadecimal uppercase
                        sp->base = 16;
                        sp->uppercase = 1;
                        vcprintf_uint(sp, va_arg(arg, unsigned));
                        break;
                    case '%':
                        // Literal
                        putc_f(data, *format);
                        ++format;
                        continue;
                    default:
                        ++format;
                        continue;
                }
                ++format;

                vcprintf_sp(data, putc_f, sp);
            }
        }
    }

    kfree(sp);
    kfree(buf);
}

void cprintf(void* data, putc_t putc_f, const char* format, ...)
{
    va_list args;

    va_start(args, format);
    vcprintf(data, putc_f, format, args);
    va_end(args);
}
