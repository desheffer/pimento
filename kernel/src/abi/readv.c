#include <sys/uio.h>

#include "abi.h"
#include "mm_context.h"
#include "page.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs_context.h"

SYSCALL_DEFINE3(readv, int, fd, const struct iovec *, iov, int, iovcnt)
{
    long res = 0;

    struct task * task = scheduler_current_task();

    struct file * file = vfs_context_file(task->vfs_context, fd);
    if (file == 0) {
        return -ENOENT;
    }

    struct page * page = page_alloc();

    while (iovcnt--) {
        off_t off = 0;
        res += vfs_read(file, page->vaddr, iov->iov_len, &off);

        mm_copy_to_user(task->mm_context, iov->iov_base, page->vaddr, off + 1);

        ++iov;
    }

    kfree(page);

    return res;
}
