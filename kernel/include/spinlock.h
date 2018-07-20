#pragma once

class SpinLock
{
  public:
    SpinLock();
    ~SpinLock();
    void acquire();
    void release();
  private:
    volatile bool _locked;
};
