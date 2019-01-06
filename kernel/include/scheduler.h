#pragma once

#include <process.h>

void scheduler_init(void);
unsigned scheduler_assign_pid(void);
void scheduler_context_switch(void);
unsigned scheduler_count(void);
struct process * scheduler_current(void);
void scheduler_destroy(struct process *);
void scheduler_enqueue(struct process *);
void scheduler_exit(struct process *);
struct process * scheduler_get_pid(unsigned);
void scheduler_tail(void);

void cpu_switch_to(struct cpu_context *, struct cpu_context *);
