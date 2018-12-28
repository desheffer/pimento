#pragma once

#include <asm-generic/unistd.h>
#include <process.h>
#include <sys/uio.h>

typedef long unsigned syscall_t();

int sys_brk(void*);
int sys_execve(const char*, char* const[], char* const[]);
void sys_exit(int);
void sys_exit_group(int);
int sys_ioctl(int, long unsigned);
ssize_t sys_read(int, char*, size_t);
ssize_t sys_readv(int, const struct iovec*, int);
long sys_set_tid_address(int*);
ssize_t sys_write(int, const char*, size_t);
ssize_t sys_writev(int, const struct iovec*, int);

void sys_invalid(unsigned);
void system_handler(registers_t*, unsigned);
