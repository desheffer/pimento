#include <abi.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>
#include <vfs_task.h>

SYSCALL_DEFINE3(write, int, fd, const char *, buf, size_t, count)
{
    long res = 0;

    struct task * task = scheduler_current_task();

    struct file * file = vfs_task_file(task, fd);
    if (file == 0) {
        return -ENOENT;
    }

    unsigned p_size = page_size();
    struct page * page = page_alloc();

    count = count < p_size ? count : p_size;
    count = mm_copy_from_user(task->mm_context, page->vaddr, buf, count);

    loff_t off = 0;
    res = vfs_write(file, page->vaddr, count, &off);

    kfree(page);

    return res;
}