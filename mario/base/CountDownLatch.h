#ifndef MARIO_BASE_COUNTDOWNLATCH_H
#define MARIO_BASE_COUNTDOWNLATCH_H

#include "mario/base/Mutex.h"
#include "mario/base/Condition.h"

namespace mario {

class CountDownLatch {

public:

    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:

    mutable MutexLock _mutex;
    Condition _condition;
    int _count;
};

}

#endif
