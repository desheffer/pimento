#pragma once

#define __NR_read 63
#define __NR_write 64
#define __NR_exit 93

extern long _syscall(long, long, long, long, long, long, long);

#define SYSCALL(n,a,b,c,d,e,f) _syscall(__NR_ ## n, (long)(a), (long)(b), (long)(c), (long)(d), (long)(e), (long)(f));

#define SYSCALL0(n) SYSCALL(n,0,0,0,0,0,0)
#define SYSCALL1(n,a) SYSCALL(n,a,0,0,0,0,0)
#define SYSCALL2(n,a,b) SYSCALL(n,a,b,0,0,0,0)
#define SYSCALL3(n,a,b,c) SYSCALL(n,a,b,c,0,0,0)
#define SYSCALL4(n,a,b,c,d) SYSCALL(n,a,b,c,d,0,0)
#define SYSCALL5(n,a,b,c,d,e) SYSCALL(n,a,b,c,d,e,0)
#define SYSCALL6(n,a,b,c,d,e,f) SYSCALL(n,a,b,c,d,e,f)
