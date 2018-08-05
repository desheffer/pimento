#include <unistd.h>
#include <stdio.h>

char* fgets(char* str, int num, FILE* stream)
{
    (void) stream;

    unsigned idx = 0;
    char c;

    while (idx < num - 1) {
        read(STDIN_FILENO, &c, 1);

        str[idx++] = c;

        if (c == '\n') {
            break;
        }
    }
    str[idx] = '\0';

    return str;
}
