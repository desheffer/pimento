#pragma once

#include <scheduler.h>

class Service
{
  public:
    static void init();
    static Service* instance();
    void handle(process_state_t*);
  private:
    static Service* _instance;

    Service();
    ~Service();
};
