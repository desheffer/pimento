#include <stdlib.h>

#ifdef __cplusplus
void* operator new(size_t size)
{
    return malloc(size);
}

void* operator new[](size_t size)
{
    return malloc(size);
}

void* operator new(size_t, void* ptr)
{
    return ptr;
}

void* operator new[](size_t, void* ptr)
{
    return ptr;
}
#endif
