#include <kstdio.h>
#include <kstring.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define BUF_MAX 17

struct vcprintf_specifiers {
    char * buf;
    unsigned alt;
    unsigned base;
    unsigned left_align;
    unsigned pad;
    unsigned uppercase;
    unsigned width;
    char length;
    char sign;
};

static void _char(struct vcprintf_specifiers * sp, char c)
{
    *sp->buf = c;
    *(sp->buf + 1) = 0;
}

static void _int(struct vcprintf_specifiers * sp, int i)
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

static void _uint(struct vcprintf_specifiers * sp, unsigned long i)
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

static void _sp(void * data, putc_t putc_f, struct vcprintf_specifiers * sp)
{
    const char * buf = sp->buf;
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

void vcprintf(void * data, putc_t putc_f, const char * format, va_list arg)
{
    struct vcprintf_specifiers sp;
    char buf[BUF_MAX];

    while (*format) {
        if (*format != '%') {
            putc_f(data, *format);
            ++format;
        } else {
            ++format;

            sp.buf = buf;
            sp.alt = 0;
            sp.base = 0;
            sp.left_align = 0;
            sp.length = 0;
            sp.pad = 0;
            sp.sign = 0;
            sp.uppercase = 0;
            sp.width = 0;

            // Flags
            while (*format) {
                switch (*format) {
                    case '-':
                        sp.left_align = 1;
                        sp.pad = 0;
                        ++format;
                        continue;
                    case '+':
                        sp.sign = '+';
                        ++format;
                        continue;
                    case '0':
                        sp.pad = 1;
                        sp.left_align = 0;
                        ++format;
                        continue;
                    case '#':
                        sp.alt = 1;
                        ++format;
                        continue;
                }
                break;
            }

            // Width
            while (*format) {
                if (*format >= '0' && *format <= '9') {
                    sp.width = (sp.width * 10) + (*format - '0');
                    ++format;
                    continue;
                } else if (*format == '*') {
                    sp.width = va_arg(arg, unsigned);
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

            // Length
            if (*format == 'l') {
                sp.length = 'l';
                ++format;
            }

            // Specifier
            if (*format) {
                switch (*format) {
                    case 'c':
                        // Character
                        _char(&sp, (char) va_arg(arg, int));
                        break;
                    case 'd':
                    case 'i':
                        // Signed decimal integer
                        sp.base = 10;
                        if (sp.length == 'l') {
                            _int(&sp, va_arg(arg, long));
                        } else {
                            _int(&sp, va_arg(arg, int));
                        }
                        break;
                    case 'p':
                        // Pointer
                        sp.alt = 1;
                        sp.base = 16;
                        sp.pad = 1;
                        sp.left_align = 0;
                        _uint(&sp, va_arg(arg, uintptr_t));
                        break;
                    case 's':
                        // String of characters
                        sp.buf = va_arg(arg, char *);
                        break;
                    case 'u':
                        // Unsigned decimal integer
                        sp.base = 10;
                        if (sp.length == 'l') {
                            _uint(&sp, va_arg(arg, unsigned long));
                        } else {
                            _uint(&sp, va_arg(arg, unsigned));
                        }
                        break;
                    case 'x':
                        // Hexadecimal lowercase
                        sp.base = 16;
                        if (sp.length == 'l') {
                            _uint(&sp, va_arg(arg, unsigned long));
                        } else {
                            _uint(&sp, va_arg(arg, unsigned));
                        }
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

                _sp(data, putc_f, &sp);
            }
        }
    }
}
