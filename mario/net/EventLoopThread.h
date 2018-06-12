#ifndef MARIO_NET_EVENTLOOPTHREAD_H
#define MARIO_NET_EVENTLOOPTHREAD_H

#include "mario/base/Mutex.h"
#include "mario/base/Condition.h"
#include "mario/base/Thread.h"

namespace mario {

class EventLoop;

class EventLoopThread {

public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop* _loop;
    bool _exiting;
    Thread _thread;
    MutexLock _mutex;
    Condition _cond;
};

}

#endif
