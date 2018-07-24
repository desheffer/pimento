#pragma once

#include <stddef.h>

typedef size_t ssize_t;

#ifdef __cplusplus
extern "C" {
#endif
    ssize_t write(int, const void*, size_t);
#ifdef __cplusplus
}
#endif
