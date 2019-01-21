#include <stdbool.h>
#include <string.h>

bool is_delim(char chr, const char * delim)
{
    while (*delim != 0) {
        if (chr == *(delim++)) {
            return true;
        }
    }

    return false;
}

char * strtok_r(char * str, const char * delim, char ** saveptr)
{
    if (str == 0) {
        str = *saveptr;
    }

    while (*str != 0 && is_delim(*str, delim)) {
        ++str;
    }

    if (*str == 0) {
        return 0;
    }

    char * end = str;
    while (*end != 0 && !is_delim(*end, delim)) {
        ++end;
    }

    *end = 0;
    *saveptr = end + 1;

    return str;
}
