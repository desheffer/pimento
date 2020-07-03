#include <stdio.h>

int main()
{
    printf("Hello from /bin/hello!\n");

    while (1) {
        char c = 0;
        scanf("%1c", &c);
        fflush(stdout);
    }

    return 0;
}
