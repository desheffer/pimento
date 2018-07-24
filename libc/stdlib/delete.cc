#include <stdlib.h>

#ifdef __cplusplus
void operator delete(void* ptr)
{
    return free(ptr);
}

void operator delete(void* ptr, size_t)
{
    return free(ptr);
}

void operator delete[](void* ptr)
{
    return free(ptr);
}

void operator delete[](void* ptr, size_t)
{
    return free(ptr);
}
#endif
