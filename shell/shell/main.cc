#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" int main()
{
    char* cmd = (char*) malloc(1024);

    while (true) {
        puts("$ ");

        unsigned idx = 0;

        while (idx < 1023) {
            cmd[idx] = getchar();
            putc(cmd[idx]);

            if (cmd[idx] == '\n') {
                break;
            }

            ++idx;
        }
        cmd[idx] = '\0';

        if (strlen(cmd) == 0) {
            continue;
        } else if (strcmp("exit", cmd) == 0) {
            break;
        } else {
            printf("Unknown command: %s\n\n", cmd);
        }
    }

    free(cmd);

    return 0;
}
