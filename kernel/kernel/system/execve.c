#include <assert.h>
#include <kstdlib.h>
#include <process.h>
#include <string.h>
#include <system.h>

static char* copy_string(const char* src)
{
    char* dest = kmalloc(strlen(src) + 1);

    strcpy(dest, src);

    return dest;
}

static char** copy_args(char* const src[])
{
    // Find size of src.
    unsigned src_size = 0;
    for (char* const* iter = src; *iter != 0; ++iter) {
        ++src_size;
    }

    char** dest = kmalloc(src_size);

    // Copy src to dest.
    for (unsigned i = 0; i < src_size; ++i) {
        dest[i] = copy_string(src[i]);
    }

    return dest;
}

int sys_execve(const char* pname, char* const argv[], char* const envp[])
{
    char* kpname = copy_string(pname);
    char** kargv = copy_args(argv);
    char** kenvp = copy_args(envp);
    // @TODO: How to free?

    process_exec(kpname, kargv, kenvp);

    return -1;
}
