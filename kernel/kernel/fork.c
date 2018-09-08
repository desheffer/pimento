#include <list.h>
#include <memory.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

process_t* process_create(const char* pname, const void* fn, const void* data)
{
    enter_critical();

    // Create a new process control block.
    process_t* process = malloc(sizeof(process_t));
    memset(process, 0, sizeof(process_t));

    // Assign a pid and basic information.
    process->pid = scheduler_assign_pid();
    process->state = created;
    strncpy(process->pname, pname, PNAME_LENGTH);

    // Initialize list of allocated pages.
    process->pages = malloc(sizeof(list_t));

    // Initialize memory map.
    mmap_create(process);

    // Allocate the first page of the stack.
    void* stack_begin = alloc_user_page(process, (void*) (STACK_TOP - PAGE_SIZE));
    void* stack_end = (char*) stack_begin + PAGE_SIZE;

    // Initialize startup parameters.
    process_regs_t* regs = (process_regs_t*) phys_to_virt(stack_end) - 1;
    regs->pstate = PSR_MODE_KTHREAD;
    regs->pc = (long unsigned) fn;
    regs->regs[0] = (long unsigned) data;
    process->sp = regs;

    // Add it to the process list and scheduling queue.
    scheduler_enqueue(process);

    leave_critical();

    return process;
}

void process_destroy(process_t* process)
{
    (void) process;

    // @TODO: Call free_page() for each entry in process->pages.
    // @TODO: Free process.
}
