#pragma once

#include <asm-generic/unistd.h>
#include <dirent.h>
#include <errno.h>
#include <kstdio.h>
#include <process.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/wait.h>

#define MAP_ARG0(...)
#define MAP_ARG1(map, vtype, vname, ...) map(vtype, vname)
#define MAP_ARG2(map, vtype, vname, ...) map(vtype, vname), MAP_ARG1(map, __VA_ARGS__)
#define MAP_ARG3(map, vtype, vname, ...) map(vtype, vname), MAP_ARG2(map, __VA_ARGS__)
#define MAP_ARG4(map, vtype, vname, ...) map(vtype, vname), MAP_ARG3(map, __VA_ARGS__)
#define MAP_ARG5(map, vtype, vname, ...) map(vtype, vname), MAP_ARG4(map, __VA_ARGS__)
#define MAP_ARG6(map, vtype, vname, ...) map(vtype, vname), MAP_ARG5(map, __VA_ARGS__)
#define MAP_ARGn(num, map, ...)          MAP_ARG ## num(map, __VA_ARGS__)

#define MAP_TYPE_NAME(vtype, vname) vtype vname
#define MAP_NAME(vtype, vname)      vname
#define MAP_U_NAME(vtype, vname)    (unsigned) (long unsigned) vname

#ifdef DEBUG_SYSCALL
# define SYSCALL_DEBUG0(name, ...) kprintf("%s()", #name)
# define SYSCALL_DEBUG1(name, ...) kprintf("%s(%#x)", #name, MAP_ARGn(1, MAP_U_NAME, __VA_ARGS__))
# define SYSCALL_DEBUG2(name, ...) kprintf("%s(%#x, %#x)", #name, MAP_ARGn(2, MAP_U_NAME, __VA_ARGS__))
# define SYSCALL_DEBUG3(name, ...) kprintf("%s(%#x, %#x, %#x)", #name, MAP_ARGn(3, MAP_U_NAME, __VA_ARGS__))
# define SYSCALL_DEBUG4(name, ...) kprintf("%s(%#x, %#x, %#x, %#x)", #name, MAP_ARGn(4, MAP_U_NAME, __VA_ARGS__))
# define SYSCALL_DEBUG5(name, ...) kprintf("%s(%#x, %#x, %#x, %#x, %#x)", #name, MAP_ARGn(5, MAP_U_NAME, __VA_ARGS__))
# define SYSCALL_DEBUG6(name, ...) kprintf("%s(%#x, %#x, %#x, %#x, %#x, %#x)", #name, MAP_ARGn(6, MAP_U_NAME, __VA_ARGS__))
# define SYSCALL_DEBUG_RET(ret)    kprintf(" = %#x\n", (unsigned) ret)
#else
# define SYSCALL_DEBUG0(name, ...)
# define SYSCALL_DEBUG1(name, ...)
# define SYSCALL_DEBUG2(name, ...)
# define SYSCALL_DEBUG3(name, ...)
# define SYSCALL_DEBUG4(name, ...)
# define SYSCALL_DEBUG5(name, ...)
# define SYSCALL_DEBUG6(name, ...)
# define SYSCALL_DEBUG_RET(ret)
#endif

#define SYSCALL_DEFINEn(num, name, ...) \
    static long _sys_ ## name(MAP_ARGn(num, MAP_TYPE_NAME, __VA_ARGS__)); \
    long sys_ ## name(MAP_ARGn(num, MAP_TYPE_NAME, __VA_ARGS__)) \
    { \
        SYSCALL_DEBUG ## num(name, __VA_ARGS__); \
        long ret = _sys_ ## name(MAP_ARGn(num, MAP_NAME, __VA_ARGS__)); \
        SYSCALL_DEBUG_RET(ret); \
        return ret; \
    } \
    long _sys_ ## name(MAP_ARGn(num, MAP_TYPE_NAME, __VA_ARGS__))

#define SYSCALL_DEFINE0(name, ...) SYSCALL_DEFINEn(0, name, __VA_ARGS__)
#define SYSCALL_DEFINE1(name, ...) SYSCALL_DEFINEn(1, name, __VA_ARGS__)
#define SYSCALL_DEFINE2(name, ...) SYSCALL_DEFINEn(2, name, __VA_ARGS__)
#define SYSCALL_DEFINE3(name, ...) SYSCALL_DEFINEn(3, name, __VA_ARGS__)
#define SYSCALL_DEFINE4(name, ...) SYSCALL_DEFINEn(4, name, __VA_ARGS__)
#define SYSCALL_DEFINE5(name, ...) SYSCALL_DEFINEn(5, name, __VA_ARGS__)
#define SYSCALL_DEFINE6(name, ...) SYSCALL_DEFINEn(6, name, __VA_ARGS__)

typedef long unsigned syscall_t();

long sys_brk(void *);
long sys_chdir(const char *);
long sys_clone(int, void *, void *, pid_t *, void *, pid_t *);
long sys_close(int);
long sys_execve(const char *, char * const[], char * const[]);
long sys_exit(int);
long sys_exit_group(int);
long sys_fcntl(int, int, int);
long sys_fstatat64(int, const char *, struct stat *, int);
long sys_getcwd(char *, size_t);
long sys_getdents64(unsigned, struct dirent *, unsigned);
long sys_geteuid(void);
long sys_getpgid(pid_t);
long sys_getpid(void);
long sys_getppid(void);
long sys_gettid(void);
long sys_getuid(void);
long sys_ioctl(int, long unsigned);
long sys_mmap(void *, size_t, int, int, int, off_t);
long sys_openat(int, const char *, int, mode_t);
long sys_read(int, char *, size_t);
long sys_readv(int, const struct iovec *, int);
long sys_rt_sigaction(int, const struct sigaction *, struct sigaction *);
long sys_rt_sigprocmask(int, const sigset_t *, sigset_t *, size_t);
long sys_rt_sigsuspend(const sigset_t *, size_t);
long sys_set_tid_address(int *);
long sys_setpgid(pid_t, pid_t);
long sys_wait4(pid_t, int *, int, struct rusage *);
long sys_write(int, const char *, size_t);
long sys_writev(int, const struct iovec *, int);

long sys_ni(unsigned);
void system_handler(struct registers *, unsigned);
