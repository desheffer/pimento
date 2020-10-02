#include "abi.h"
#include "mm_context.h"
#include "page.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs_context.h"

SYSCALL_DEFINE3(read, int, fd, char *, buf, size_t, count)
{
    long res = 0;

    struct task * task = scheduler_current_task();

    struct file * file = vfs_context_file(task->vfs_context, fd);
    if (file == 0) {
        return -ENOENT;
    }

    struct page * page = page_alloc();

    off_t off = 0;
    res = vfs_read(file, page->vaddr, count, &off);

    mm_copy_to_user(task->mm_context, buf, page->vaddr, off + 1);

    kfree(page);

    return res;
}
