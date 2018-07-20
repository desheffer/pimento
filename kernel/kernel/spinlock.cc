#include <assert.h>
#include <spinlock.h>
#include <synchronize.h>

SpinLock::SpinLock()
{
    _locked = false;
}

SpinLock::~SpinLock()
{
    assert(!_locked);
}

void SpinLock::acquire()
{
    bool acquired = false;

    while (!acquired) {
        enter_critical();

        if (!_locked) {
            _locked = true;
            acquired = true;
        }

        leave_critical();
    }
}

void SpinLock::release()
{
    assert(_locked);

    enter_critical();

    _locked = false;

    leave_critical();
}
