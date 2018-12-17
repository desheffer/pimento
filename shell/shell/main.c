#include <stdio.h>
#include <string.h>

int main()
{
    printf("[Shell]\n\n");

    char cmd[1024];

    while (1) {
        printf("$ ");
        fflush(stdout);

        fgets(cmd, 1024, stdin);

        if (strcmp("\n", cmd) == 0) {
            continue;
        } else if (strcmp("exit\n", cmd) == 0) {
            break;
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }

    return 0;
}
