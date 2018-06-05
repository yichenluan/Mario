#ifndef MARIO_BASE_CONDITION_H
#define MARIO_BASE_CONDITION_H

#include "mario/base/Mutex.h"

#include <pthread.h>

namespace mario {


class Condition {
    
public:
    explicit Condition(MutexLock& mutex)
        :_mutex(mutex) {
        pthread_cond_init(&_pcond, NULL);
    }

    ~Condition() {
        pthread_cond_destroy(&_pcond);
    }

    void wait() {
        pthread_cond_wait(&_pcond, _mutex.getPthreadMutex());
    }

    void notify() {
        pthread_cond_signal(&_pcond);
    }

    void notifyAll() {
        pthread_cond_broadcast(&_pcond);
    }

private:
    MutexLock& _mutex;
    pthread_cond_t _pcond;
};

}

#endif
