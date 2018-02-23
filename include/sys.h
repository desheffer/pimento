#ifndef SYS_H
#define SYS_H

extern "C" void halt();
extern "C" void panic();
extern "C" void exception_handler(uint64_t, uint64_t);

#endif
