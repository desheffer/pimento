#pragma once

#include <unistd.h>

#define SYSCALL_COUNT 292

typedef long syscall_t(long, long, long, long, long, long);

void system_init();
long system_handler(unsigned, long, long, long, long, long, long);
void system_register_call(unsigned, syscall_t*);

void do_exit(int);
ssize_t do_read(int, void*, size_t);
ssize_t do_write(int, const void*, size_t);

void do_invalid();
