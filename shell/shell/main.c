#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[])
{
    (void) argc;

    printf("[Shell]\n\n");

    char cmd[1024];

    while (1) {
        printf("$ ");
        fflush(stdout);

        fgets(cmd, 1024, stdin);

        char* pos = strchr(cmd, '\n');
        if (pos != 0) {
            *pos = '\0';
        }

        if (strcmp("", cmd) == 0) {
            continue;
        } else if (strcmp("env", cmd) == 0) {
            for (int i = 0; envp[i] != 0; ++i) {
                printf("%s\n", envp[i]);
            }
        } else if (strcmp("exit", cmd) == 0) {
            break;
        } else {
            printf("%s: %s: unknown command\n", argv[0], cmd);
        }
    }

    return 0;
}
