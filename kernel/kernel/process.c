#include <assert.h>
#include <fs.h>
#include <kstdlib.h>
#include <memory.h>
#include <mm.h>
#include <process.h>
#include <scheduler.h>
#include <string.h>
#include <synchronize.h>

struct process * process_create_common(const char * pname, int ppid, int pid, bool init_kstack)
{
    // Create a new process control block.
    struct process * child = kzalloc(sizeof(struct process));

    // Assign a pid.
    if (pid >= 0) {
        child->pid = pid;
    } else {
        child->pid = scheduler_assign_pid();
    }

    // Assign basic information.
    child->state = created;
    child->ppid = ppid;
    strncpy(child->pname, pname, PNAME_LENGTH);

    // Initialize a new memory map.
    mm_process_create(child);
    if (init_kstack) {
        mm_create_kstack(child);
    }

    // Copy filesystem information.
    fs_process_create(child);

    // Initialize execution.
    child->cpu_context = kzalloc(sizeof(struct cpu_context));

    return child;
}

struct process * process_create_kernel(void)
{
    return process_create_common("kernel", 0, -1, false);
}

static void process_create_tail(process_function_t fn, void * data)
{
    scheduler_tail();

    fn(data);

    // Exit the process when it is finished.
    struct process * process = scheduler_current();
    scheduler_exit(process);

    scheduler_context_switch();
}

int process_create(void * fn, const char * pname, void * data)
{
    enter_critical();

    struct process * child = process_create_common(pname, 0, -1, true);

    // Initialize execution.
    child->cpu_context->sp = (long unsigned) KSTACK_TOP;
    child->cpu_context->pc = (long unsigned) process_tail_wrapper;
    child->cpu_context->regs[0] = (long unsigned) process_create_tail;
    child->cpu_context->regs[1] = (long unsigned) fn;
    child->cpu_context->regs[2] = (long unsigned) data;

    scheduler_enqueue(child);

    leave_critical();

    return child->pid;
}

int process_clone(struct process * parent)
{
    enter_critical();

    struct process * child = process_create_common(parent->pname, parent->pid, -1, false);

    // Copy the memory map.
    mm_copy_from(parent, child);

    // Initialize execution.
    child->cpu_context->sp = (long unsigned) KSTACK_TOP - PROCESS_REGS_SIZE;
    child->cpu_context->pc = (long unsigned) process_tail_wrapper;
    child->cpu_context->regs[0] = (long unsigned) process_create_tail;
    child->cpu_context->regs[1] = (long unsigned) do_exec;
    child->cpu_context->regs[2] = (long unsigned) KSTACK_TOP - PROCESS_REGS_SIZE;

    scheduler_enqueue(child);

    leave_critical();

    return child->pid;
}

void process_destroy(struct process * process)
{
    fs_process_destroy(process);
    mm_process_destroy(process);
    kfree(process->cpu_context);

    // @TODO
    /* kfree(process); */
}
