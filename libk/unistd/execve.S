#include <asm-generic/unistd.h>

.globl execve
execve:
    ldr x8, =__NR_execve
    svc #0
    ret
