#pragma once

class Serial
{
  public:
    static void init();
    static char getc();
    static void putc(char);
  private:
    Serial();
    ~Serial();
};
