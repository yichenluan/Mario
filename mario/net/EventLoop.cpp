#include "mario/net/EventLoop.h"

#include "mario/net/Channel.h"
#include "mario/net/Poller.h"
#include "mario/base/easylogging++.h"
#include "mario/net/TimerQueue.h"

#include <sys/eventfd.h>
#include <unistd.h>

// TODO. Select/Poll/Epoll Diff And Why.
//#include <poll.h>

using namespace mario;

__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

static int createEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOG(FATAL) << "Failed in eventfd";
    }
    return evtfd;
}

EventLoop::EventLoop() 
	: _looping(false)
    , _poller(new Poller(this))
    , _timerQueue(new TimerQueue(this))
	, _threadId(CurrentThread::tid()) 
    , _callingPendingFunctors(false)
    , _wakeupFd(createEventfd())
    , _wakeupChannel(new Channel(this, _wakeupFd))
{

	LOG(INFO) << "EventLoop created" << this << " in thread " << _threadId;
	if (t_loopInThisThread) {
		LOG(FATAL) << "Another EventLoop " << t_loopInThisThread
				  << " exists in this thread " << _threadId;
	} else {
		t_loopInThisThread = this;
	}

    _wakeupChannel -> setReadCallback(
            std::bind(&EventLoop::handleRead, this)
        );
    _wakeupChannel -> enableReading();
}

EventLoop::~EventLoop() {
	t_loopInThisThread = NULL;
}

void EventLoop::loop() {
	assertInLoopThread();
	_looping = true;
    _quit = false;

    while (!_quit) {
        _activeChannels.clear();
        _pollReturnTime = _poller->poll(kPollTimeMs, &_activeChannels);
        for (auto channel_p : _activeChannels) {
            channel_p->handleEvent(_pollReturnTime);
        }

        doPendingFunctors();
    }

	LOG(INFO) << "EventLoop" << this << " stop looping";
	_looping = false;
}

void EventLoop::quit() {
    _quit = true;
}

void EventLoop::updateChannel(Channel* channel) {
    assertInLoopThread();
    _poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    assertInLoopThread();
    _poller->removeChannel(channel);
}

void EventLoop::abortNotInLoopThread() {
	LOG(FATAL) << "EventLoop::abortNotInLoopThread - EventLoop " << this
			  << " was created in _threadId = " << _threadId
			  << ", current thread id = " << CurrentThread::tid();
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb) {
    return _timerQueue->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb) {
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb) {
    Timestamp time(addTime(Timestamp::now(), interval));
    return _timerQueue->addTimer(cb, time, interval);
}

void EventLoop::runInLoop(const Functor& cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor& cb) {
    {
        MutexLockGuard lock(_mutex);
        _pendingFunctors.push_back(cb);
    }

    if (!isInLoopThread() || _callingPendingFunctors) {
        wakeup();
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(_wakeupFd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG(FATAL) << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(_wakeupFd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG(FATAL) << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    _callingPendingFunctors = true;
    {
        MutexLockGuard lock(_mutex);
        functors.swap(_pendingFunctors);
    }

    for (auto functor : functors) {
        functor();
    }
    _callingPendingFunctors = false;
}
