#pragma once

#include <process.h>

void scheduler_init(void);
unsigned scheduler_assign_pid(void);
void scheduler_context_switch(void);
unsigned scheduler_count(void);
process_t* scheduler_current(void);
void scheduler_destroy(process_t*);
void scheduler_enqueue(process_t*);
void scheduler_exit(process_t*);
process_t* scheduler_get_pid(unsigned);
void scheduler_tail(void);

void cpu_switch_to(cpu_context_t*, cpu_context_t*);
