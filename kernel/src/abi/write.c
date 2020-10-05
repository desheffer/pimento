#include "abi.h"
#include "mm_context.h"
#include "page.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs_context.h"

SYSCALL_DEFINE3(write, int, fd, const char *, buf, size_t, count)
{
    long res = 0;

    struct task * task = scheduler_current_task();

    struct file * file = vfs_context_file(task->vfs_context, fd);
    if (file == 0) {
        return -EBADF;
    }

    unsigned p_size = page_size();
    struct page * page_buf = page_alloc();

    count = count < p_size ? count : p_size;
    count = mm_copy_from_user(task->mm_context, page_buf->vaddr, buf, count);

    off_t off = 0;
    res = vfs_write(file, page_buf->vaddr, count, &off);

    kfree(page_buf);

    return res;
}
