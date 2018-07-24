#include <stdint.h>
#include <stdio.h>

size_t vsprintf(char* s, const char* format, va_list arg)
{
    return vsnprintf(s, UINT32_MAX, format, arg);
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
