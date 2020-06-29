#include <abi.h>
#include <pimento.h>
#include <vfs.h>

SYSCALL_DEFINE3(write, int, fd, const char *, buf, size_t, count)
{
    long res = 0;

    struct path * path = vfs_path_create();
    struct file * file = vfs_file_create();

    // @TODO
    if (fd != 1) {
        kputs("fd != 1 not supported\n");
        return -999;
    }

    vfs_resolve_path(path, 0, "/dev/ttyS0");

    // @TODO: use file from e.g. `openat`
    int res2 = vfs_open(path, file);
    if (res2 < 0) {
        vfs_path_destroy(path);
        vfs_file_destroy(file);

        return res;
    }

    loff_t off = 0;
    res = vfs_write(file, buf, count, &off);

    vfs_path_destroy(path);
    vfs_file_destroy(file);

    return res;
}
