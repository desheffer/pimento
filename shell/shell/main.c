#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char * argv[], char * envp[])
{
    (void) argc;

    printf("[Shell]\n\n");

    char cmd[1024];

    while (1) {
        printf("$ ");
        fflush(stdout);

        fgets(cmd, 1024, stdin);

        char * pos = strchr(cmd, '\n');
        if (pos != 0) {
            *pos = '\0';
        }

        if (strcmp("", cmd) == 0) {
            continue;
        } else if (strcmp("help", cmd) == 0) {
            printf("env  - display environment\n");
            printf("exit - quit the shell\n");
            printf("help - show this message\n");
            printf("pwd  - print the current directory\n");
            printf("sh   - test fork() and execve()\n");
        } else if (strcmp("env", cmd) == 0) {
            for (int i = 0; envp[i] != 0; ++i) {
                printf("%s\n", envp[i]);
            }
        } else if (strcmp("exit", cmd) == 0) {
            break;
        } else if (strcmp("pwd", cmd) == 0) {
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
        } else if (strcmp("sh", cmd) == 0) {
            unsigned pid = fork();

            if (pid == 0) {
                execve(argv[0], argv, envp);
                return 0;
            }

            waitpid(pid, 0, 0);
        } else {
            printf("%s: %s: unknown command\n", argv[0], cmd);
        }
    }

    return 0;
}
