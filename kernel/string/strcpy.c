#include <string.h>

char * strcpy(char * dest, const char * src)
{
    return (char *) memcpy(dest, src, strlen(src) + 1);
}
