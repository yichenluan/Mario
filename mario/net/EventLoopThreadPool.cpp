#include "mario/net/EventLoopThreadPool.h"

#include "mario/net/EventLoop.h"
#include "mario/net/EventLoopThread.h"

#include <functional>

using namespace mario;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
    : _baseLoop(baseLoop)
    , _started(false)
    , _numThreads(0)
    , _next(0)
{}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::start() {
    _baseLoop->assertInLoopThread();
    _started = true;

    for (int i = 0; i < _numThreads; ++i) {
        EventLoopThread* t = new EventLoopThread;
        _threads.push_back(t);
        _loops.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    _baseLoop->assertInLoopThread();
    EventLoop* loop = _baseLoop;

    if (!_loops.empty()) {
        loop = _loops[_next];
        ++_next;
        if (_next >= _loops.size()) {
            _next = 0;
        }
    }
    return loop;
}
