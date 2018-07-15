#pragma once

class Serial
{
  public:
    static void init();
    static Serial* instance() { return _instance; }
    static char getc();
    static void putc(void*, char);
  private:
    static Serial* _instance;

    Serial();
    ~Serial();
};
