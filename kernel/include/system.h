#pragma once

#include <asm-generic/unistd.h>
#include <process.h>
#include <signal.h>
#include <sys/uio.h>
#include <sys/wait.h>

typedef long unsigned syscall_t();

long sys_brk(void *);
long sys_clone(int, void *, void *, pid_t *, void *, pid_t *);
long sys_execve(const char *, char * const[], char * const[]);
long sys_exit(int);
long sys_exit_group(int);
long sys_geteuid(void);
long sys_gettid(void);
long sys_getuid(void);
long sys_ioctl(int, long unsigned);
long sys_mmap(void *, size_t, int, int, int, off_t);
long sys_read(int, char *, size_t);
long sys_readv(int, const struct iovec *, int);
long sys_rt_sigprocmask(int, const sigset_t *, sigset_t *, size_t);
long sys_set_tid_address(int *);
long sys_wait4(pid_t, int *, int, struct rusage *);
long sys_write(int, const char *, size_t);
long sys_writev(int, const struct iovec *, int);

long sys_ni(unsigned);
void system_handler(struct registers *, unsigned);
