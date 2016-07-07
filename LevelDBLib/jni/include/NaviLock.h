
#ifndef __NAVI_LOCK_H__
#define __NAVI_LOCK_H__

#include <pthread.h>

class NaviLock
{
public:
    NaviLock()
    {
        pthread_mutex_init(&_mutex, NULL);
    }
   
    ~NaviLock()
    {
        pthread_mutex_destroy(&_mutex);
    }

    bool lock()
    {
        int nRetVal = pthread_mutex_lock(&_mutex);
        if (0 != nRetVal) {
            return false;
        }

        return true;
    }

    bool unLock()
    {
        pthread_mutex_unlock(&_mutex);
        
        return true;
    }

private:
    pthread_mutex_t _mutex;
};

#endif // __NAVI_LOCK_H__

