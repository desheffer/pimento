#pragma once

#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 4096

struct page_t {
    bool allocated: 1;
};

class Memory
{
  public:
    static void init();
    static Memory* instance() { return _instance; }
    void* allocPage();
    void freePage(void*);
    unsigned allocSize() const;
    unsigned pageCount() const;

  private:
    static Memory* _instance;
    size_t _allocSize;
    unsigned _pageCount;
    page_t* _pages;
    unsigned _firstCandidate;

    Memory(size_t);
    ~Memory();
    void* pageStart(unsigned) const;
    unsigned pageIndex(void*) const;
};

void* alloc_page();
void free_page(void*);
