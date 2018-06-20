#ifndef MARIO_NET_EVENTLOOPTHREADPOOL_H
#define MARIO_NET_EVENTLOOPTHREADPOOL_H

#include "mario/base/Condition.h"
#include "mario/base/Mutex.h"
#include "mario/base/Thread.h"

#include <vector>

namespace mario {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool {

public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) {
        _numThreads = numThreads;
    }

    void start();

    EventLoop* getNextLoop();

private:
        EventLoop* _baseLoop;
        bool _started;
        int _numThreads;
        int _next;

        std::vector<EventLoop*> _loops;
        std::vector<EventLoopThread*> _threads;
};

}

#endif
