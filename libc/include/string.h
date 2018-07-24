#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
    void* memcpy(void*, const void*, size_t);
    void* memset(void*, int, size_t);
    int32_t strcmp(const char*, const char*);
    char* strcpy(char*, const char*);
    size_t strlen(const char*);
    char* strncpy(char*, const char*, size_t);
#ifdef __cplusplus
}
#endif
