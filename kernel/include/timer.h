#pragma once

#include <bcm2837.h>
#include <interrupt.h>

#define HZ 100

void timer_init(void);
void timer_connect(interrupt_handler_t*, void*);
void timer_disconnect(void);
void timer_reset(void);
long unsigned timer_counter(void);
