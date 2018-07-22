#pragma once

class Serial
{
  public:
    static void init();
    static Serial* instance();
    static char getc();
    static void putc(char);
  private:
    static Serial* _instance;

    Serial();
    ~Serial();
};
