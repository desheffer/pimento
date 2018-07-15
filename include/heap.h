#pragma once

#include <memory.h>
#include <stddef.h>

class Heap
{
  public:
    static void init(Memory*);
    static Heap* instance() { return _instance; }
    void* alloc(size_t);
    void free(void*);
  private:
    static Heap* _instance;
    Memory* _memory;

    Heap(Memory*);
    ~Heap();
};
