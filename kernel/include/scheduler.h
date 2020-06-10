#pragma once

#include <task.h>

#define SCHEDULER_TIMER 10

typedef void (* timer_set_t)(unsigned);

void scheduler_init(timer_set_t);
struct task * scheduler_current_task(void);
void scheduler_enqueue(struct task *);
void scheduler_replace(struct task *, struct task *);
void schedule(void);

void switch_to(struct task *, struct task *);
