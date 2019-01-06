#pragma once

#include <asm-generic/unistd.h>
#include <process.h>
#include <signal.h>
#include <sys/uio.h>
#include <sys/wait.h>

typedef long unsigned syscall_t();

void* sys_brk(void*);
int sys_clone(int, void*, void*, pid_t*, void*, pid_t*);
int sys_execve(const char*, char* const[], char* const[]);
void sys_exit(int);
void sys_exit_group(int);
uid_t sys_geteuid(void);
pid_t sys_gettid(void);
uid_t sys_getuid(void);
int sys_ioctl(int, long unsigned);
void* sys_mmap(void*, size_t, int, int, int, off_t);
ssize_t sys_read(int, char*, size_t);
ssize_t sys_readv(int, const struct iovec*, int);
int sys_rt_sigprocmask(int, const sigset_t*, sigset_t*, size_t);
long sys_set_tid_address(int*);
void sys_wait4(pid_t, int*, int, struct rusage*);
ssize_t sys_write(int, const char*, size_t);
ssize_t sys_writev(int, const struct iovec*, int);

int sys_ni(unsigned);
void system_handler(struct registers *, unsigned);
