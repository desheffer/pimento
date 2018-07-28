#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    void exit(int status);
    void free(void*);
    void* malloc(size_t);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
void operator delete(void*);
void operator delete[](void*);
void* operator new(size_t);
void* operator new[](size_t);
void* operator new(size_t, void*);
#endif
