#pragma once

#include <process.h>

void scheduler_init();
short unsigned scheduler_assign_pid();
void scheduler_context_switch();
unsigned scheduler_count();
process_t* scheduler_current();
void scheduler_destroy(process_t*);
void scheduler_enqueue(process_t*);
void scheduler_tail();

void cpu_switch_to(cpu_context_t*, cpu_context_t*);
void fork_tail();
