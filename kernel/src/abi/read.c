#include <abi.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>
#include <vfs_task.h>

SYSCALL_DEFINE3(read, int, fd, char *, buf, size_t, count)
{
    long res = 0;

    struct task * task = scheduler_current_task();

    struct file * file = vfs_task_file(task, fd);
    if (file == 0) {
        return -ENOENT;
    }

    struct page * page = page_alloc();

    loff_t off = 0;
    res = vfs_read(file, page->vaddr, count, &off);

    mm_copy_to_user(task->mm_context, buf, page->vaddr, off + 1);

    kfree(page);

    return res;
}
