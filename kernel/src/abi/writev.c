#include <abi.h>
#include <pimento.h>
#include <scheduler.h>
#include <sys/uio.h>
#include <task.h>
#include <vfs_task.h>

SYSCALL_DEFINE3(writev, int, fd, const struct iovec *, iov, int, iovcnt)
{
    long res = 0;

    struct task * task = scheduler_current_task();

    struct file * file = vfs_task_file(task, fd);
    if (file == 0) {
        return -ENOENT;
    }

    unsigned p_size = page_size();
    struct page * page = page_alloc();

    while (iovcnt--) {
        size_t count = iov->iov_len < p_size ? iov->iov_len : p_size;
        count = mm_copy_from_user(task->mm_context, page->vaddr, iov->iov_base, count);

        loff_t off = 0;
        res += vfs_write(file, page->vaddr, count, &off);

        ++iov;
    }

    kfree(page);

    return res;
}
