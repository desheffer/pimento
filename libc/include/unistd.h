#pragma once

#include <stddef.h>

typedef size_t ssize_t;

#define SYS_READ 0
#define SYS_WRITE 1

#ifdef __cplusplus
extern "C" {
#endif
    ssize_t read(int, void*, size_t);
    ssize_t write(int, const void*, size_t);
#ifdef __cplusplus
}
#endif
