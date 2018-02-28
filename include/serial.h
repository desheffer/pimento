#pragma once

class Serial
{
  public:
    static Serial* instance();
    void init();
    static char getc();
    static void putc(void*, char);
  private:
    static Serial* _instance;

    Serial();
    ~Serial();
};
