#pragma once

extern "C" void halt();

extern "C" void __dso_handle();
extern "C" void __cxa_atexit(void*, void (*)(void*), void*);
