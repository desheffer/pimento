#include <assert.h>
#include <list.h>
#include <memory.h>
#include <process.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

static unsigned _next_pid;
static list_t* _process_list = 0;

static unsigned process_create_init()
{
    enter_critical();

    process_control_block_t* process = malloc(sizeof(process_control_block_t));
    process->pid = _next_pid++;
    process->state = created;
    strcpy(process->pname, "init");

    push_back(_process_list, process);

    scheduler_add(process);

    leave_critical();

    return process->pid;
}

void process_init()
{
    _process_list = malloc(sizeof(list_t));
    _process_list->front = 0;
    _process_list->back = 0;

    _next_pid = 1;

    process_create_init();
    scheduler_start();
}

unsigned process_count()
{
    return count(_process_list);
}

unsigned process_create(const char* pname, const void* lr)
{
    enter_critical();

    process_control_block_t* process = malloc(sizeof(process_control_block_t));
    process->pid = _next_pid++;
    process->state = created;
    strncpy(process->pname, pname, 32);
    process->pname[31] = '\0';

    process->stack_begin = alloc_page();
    process->stack_end = (char*) process->stack_begin + PAGE_SIZE;

    // @TODO: Allow stack to extend beyond one page.
    process->regs = (process_regs_t*) process->stack_end - 1;
    process->regs->spsr = 0x300;
    process->regs->lr = (long unsigned) lr;

    push_back(_process_list, process);

    scheduler_add(process);

    leave_critical();

    return process->pid;
}

void process_destroy(unsigned pid)
{
    enter_critical();

    process_control_block_t* process = process_lookup(pid);

    assert(process->state == stopping);

    remove(_process_list, process);

    leave_critical();
}

process_control_block_t* process_lookup(unsigned pid)
{
    list_item_t* list_item = _process_list->front;
    process_control_block_t* process = 0;

    while (list_item != 0) {
        process = (process_control_block_t*) list_item->item;
        if (process->pid == pid) {
            return process;
        }

        list_item = list_item->next;
    }

    return 0;
}

void process_stop(unsigned pid)
{
    enter_critical();

    process_control_block_t* process = process_lookup(pid);
    process->state = stopping;

    leave_critical();
}
