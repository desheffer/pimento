#include <fs.h>
#include <scheduler.h>
#include <string.h>
#include <system.h>

static int getcwd_recursive(struct dentry * dentry, char * buf, size_t count)
{
    int ret = 0;

    if (dentry->dentry_parent != 0) {
        // @TODO: Recursion is not a good idea with a fixed stack.
        ret = getcwd_recursive(dentry->dentry_parent, buf, count);

        strncpy(buf + ret, "/", count - ret);
        ret += 1;
    }

    strncpy(buf + ret, dentry->name, count - ret);
    ret += strlen(buf + ret);

    return ret;
}

SYSCALL_DEFINE2(getcwd, char *, buf, size_t, count)
{
    struct process * process = scheduler_current();

    struct dentry * cwd = process->fs_context->cwd;

    if (cwd->dentry_parent == 0) {
        strncpy(buf, "/", count);
    } else {
        getcwd_recursive(cwd, buf, count);
    }

    return strlen(buf) + 1;
}
