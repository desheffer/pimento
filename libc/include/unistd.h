#pragma once

#include <stddef.h>
#include <sys/syscall.h>

typedef size_t ssize_t;

#ifdef __cplusplus
extern "C" {
#endif
    void exit(int);
    ssize_t read(int, void*, size_t);
    ssize_t write(int, const void*, size_t);
#ifdef __cplusplus
}
#endif
