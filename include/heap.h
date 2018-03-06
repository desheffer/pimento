#pragma once

#include <memory.h>
#include <stddef.h>

class Heap
{
  public:
    static Heap* instance();
    void init(Memory*);
    void* alloc(size_t);
    void free(void*);
  private:
    static Heap* _instance;
    Memory* _memory;

    Heap();
    ~Heap();
};
