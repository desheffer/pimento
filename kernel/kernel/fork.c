#include <list.h>
#include <memory.h>
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
    process->pid = process_assign_pid();
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
    process->regs = (process_regs_t*) phys_to_virt(stack_end) - 1;
    process->regs->pstate = PSR_MODE_KTHREAD;
    process->regs->pc = (long unsigned) fn;
    process->regs->regs[0] = (long unsigned) data;

    // Add it to the process list and scheduling queue.
    process_enqueue(process);

    leave_critical();

    return process;
}
