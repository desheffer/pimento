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
    static inline Memory* instance() { return _instance; }
    void reserveRange(void*, void*);
    void* allocPage();
    void freePage(void*);
    unsigned pageCount() const;
    size_t pageSize() const;

  private:
    static Memory* _instance;
    void* _allocStart;
    size_t _pageSize;
    unsigned _pageCount;
    page_t* _pages;
    unsigned _lastIndex;

    Memory(void*, void*, size_t);
    ~Memory();
    int pageIndex(void*) const;
    void* pageStart(unsigned) const;
};
