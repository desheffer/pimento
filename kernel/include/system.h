#pragma once

#include <unistd.h>

#define SYSCALL_COUNT 292

typedef long syscall_t(long, long, long, long, long, long);

class System
{
  public:
    static void init();
    static inline System* instance() { return _instance; }
    long handle(unsigned, long, long, long, long, long, long) const;
    void registerCall(unsigned, syscall_t*);
  private:
    static System* _instance;
    syscall_t* _calls[SYSCALL_COUNT];

    System();
    ~System();
};

extern "C" long system_handler(unsigned, long, long, long, long, long, long);

void do_exit(int);
ssize_t do_read(int, void*, size_t);
ssize_t do_write(int, const void*, size_t);

void do_invalid();
