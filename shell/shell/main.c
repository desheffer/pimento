#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[], char* envp[])
{
    (void) argc;

    /* if (fork() == 0) { */
    /*     printf("hello from child\n"); */
    /*     while (1); */
    /*     return 0; */
    /* } else { */
    /*     printf("hello from parent\n"); */
    /* } */

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
        } else if (strcmp("sh", cmd) == 0) {
            execve(argv[0], argv, envp);
        } else if (strcmp("fork", cmd) == 0) {
            if (fork() == 0) {
                printf("hello from child\n");
                return 0;
            }
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
