#pragma once

#include <stddef.h>

typedef size_t ssize_t;

#define SYS_EXIT 0
#define SYS_READ 1
#define SYS_WRITE 2

#ifdef __cplusplus
extern "C" {
#endif
    void exit(int);
    ssize_t read(int, void*, size_t);
    ssize_t write(int, const void*, size_t);
#ifdef __cplusplus
}
#endif
