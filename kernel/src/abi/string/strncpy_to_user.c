#include <abi/string.h>
#include <task.h>

size_t strncpy_to_user(struct mm_context * mm_context, void * dest,
                       const char * src, size_t num)
{
    unsigned length = strlen(src) + 1;

    num = length < num ? length : num;

    return mm_copy_to_user(mm_context, dest, src, num);
}
