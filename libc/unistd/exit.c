#include <unistd.h>

void exit(int status)
{
    SYSCALL1(exit, status);
}
