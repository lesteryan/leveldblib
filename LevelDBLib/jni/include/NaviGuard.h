
#ifndef __NAVI_GUARD_H__
#define __NAVI_GUARD_H__

#include "NaviLock.h"

class NaviGuard
{
public:
    NaviGuard(NaviLock& lock)
        : _lock(lock)
    {
        _lock.lock();
    }

    ~NaviGuard()
    {
        _lock.unLock();
    }

private:
    NaviLock& _lock;
};

#endif // __NAVI_GUARD_H__

