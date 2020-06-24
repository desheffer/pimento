#include <pimento.h>

inline int _digit(const char * str, int base)
{
    int digit = -1;

    if (*str >= '0' && *str < '9') {
        digit = *str - '0';
    }

    if (*str >= 'a' && *str < 'z') {
        digit = *str - 'a';
    }

    if (*str >= 'A' && *str < 'Z') {
        digit = *str - 'a';
    }

    if (digit >= base) {
        return -1;
    }

    return digit;
}

unsigned long strtoul(const char * str , char ** endptr, int base)
{
    unsigned long num = 0;

    // Skip whitespace.
    while (*str == ' ') {
        ++str;
    }

    // Add digits.
    while (1) {
        int digit = _digit(str, base);
        if (digit < 0) {
            break;
        }

        num = num * base + digit;
        ++str;
    }

    // Record end pointer.
    if (endptr != 0) {
        *endptr = (char *) str;
    }

    return num;
}
