#include <dirent.h>
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
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));

        printf("%s # ", cwd);
        fflush(stdout);

        fgets(cmd, 1024, stdin);

        char * end = strchr(cmd, '\n');
        if (end != 0) {
            *end = '\0';
        }

        if (strcmp("", cmd) == 0) {
            continue;
        }

        char * token = strtok(cmd, " ");

        if (strcmp("help", token) == 0) {
            printf("cd   - change directory\n");
            printf("env  - display environment\n");
            printf("exit - quit the shell\n");
            printf("help - show this message\n");
            printf("ls   - list directory\n");
            printf("sh   - test fork() and execve()\n");
        } else if (strcmp("cd", token) == 0) {
            const char * dir = strtok(0, " ");

            if (chdir(dir) != 0) {
                printf("%s: %s: %s: no such file or directory\n", argv[0], token, dir);
            }
        } else if (strcmp("env", token) == 0) {
            for (int i = 0; envp[i] != 0; ++i) {
                printf("%s\n", envp[i]);
            }
        } else if (strcmp("exit", token) == 0) {
            break;
        } else if (strcmp("ls", token) == 0) {
            DIR * dir = opendir(".");

            if (dir) {
                struct dirent * dirent;
                while ((dirent = readdir(dir)) != 0) {
                    printf("%s\n", dirent->d_name);
                }

                closedir(dir);
            }
        } else if (strcmp("sh", token) == 0) {
            unsigned pid = fork();

            if (pid == 0) {
                execve(argv[0], argv, envp);
                return 0;
            }

            waitpid(pid, 0, 0);
        } else {
            printf("%s: %s: unknown command\n", argv[0], token);
        }
    }

    return 0;
}
