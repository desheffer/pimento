#include <assert.h>
#include <kstdio.h>
#include <kstdlib.h>

struct vsnprintf_putc_data {
    char* dest;
    size_t num_chars;
    size_t max_chars;
};

static void vsnprintf_putc(void* data, char c)
{
    struct vsnprintf_putc_data* putc_data = (struct vsnprintf_putc_data*) data;

    if (putc_data->num_chars < putc_data->max_chars) {
        putc_data->dest[putc_data->num_chars++] = c;
    }
}

size_t vsnprintf(char* s, size_t max, const char* format, va_list arg)
{
    struct vsnprintf_putc_data* putc_data = (struct vsnprintf_putc_data*) kmalloc(sizeof(struct vsnprintf_putc_data));

    putc_data->dest = s;
    putc_data->num_chars = 0;
    putc_data->max_chars = max;

    vcprintf(putc_data, &vsnprintf_putc, format, arg);

    putc_data->dest[putc_data->num_chars] = 0;

    unsigned num_chars = putc_data->num_chars;
    kfree(putc_data);

    return num_chars;
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
