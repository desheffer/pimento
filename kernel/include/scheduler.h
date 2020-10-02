#pragma once

#include "task.h"

#define SCHEDULER_TIMER 10

typedef void (* timer_set_t)(unsigned);

void scheduler_init(timer_set_t);
unsigned scheduler_count_children(struct task *);
struct task * scheduler_current_task(void);
void scheduler_enqueue(struct task *);
void scheduler_replace(struct task *, struct task *);
void scheduler_schedule(void);
struct task * scheduler_wait(struct task *, pid_t);

void switch_to(struct task *, struct task *);
