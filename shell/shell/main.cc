#include <stdio.h>

extern "C" int main()
{
    puts("$ ");

    while (true) {
        char c = getchar();
        printf("%c", c);
    }
}
