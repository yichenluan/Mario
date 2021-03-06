#include "mario/net/TimerQueue.h"

#include "mario/base/easylogging++.h"
#include "mario/net/EventLoopThread.h"
#include "mario/net/EventLoop.h"
#include "mario/net/Timer.h"
#include "mario/net/TimerId.h"

#include <functional>
#include <sys/timerfd.h>
#include <unistd.h>


namespace mario {

namespace detail {

int createTimerfd() {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        LOG(FATAL) << "Failed in timerfd_create";
    }

    return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when) {
    int64_t microseconds = when.microSecondsSinceEpoch()
        - Timestamp::now().microSecondsSinceEpoch();
    if (microseconds < 100) {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);

    return ts;
}

void readTimerfd(int timerfd) {
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));

    //LOG(INFO) << "TimerQueue::handleRead() " << howmany;

    if (n != sizeof(howmany)) {
        LOG(FATAL) << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
    }
}

void resetTimerfd(int timerfd, Timestamp expiration) {
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));

    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret) {
        LOG(FATAL) << "timerfd_settime()";
    }
}

}
}

using namespace mario;
using namespace mario::detail;

TimerQueue::TimerQueue(EventLoop* loop)
    : _loop(loop)
    , _timerfd(createTimerfd()) 
    , _timerfdChannel(loop, _timerfd)
    , _timers()
    , _callingExpiredTimers(false)
{
    _timerfdChannel.setReadCallback(
            std::bind(&TimerQueue::handleRead, this)
        );
    _timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue() {
    ::close(_timerfd);
    for (auto it = _timers.begin(); it != _timers.end(); ++it) {
        delete it->second;
    }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, Timestamp when, double interval) {
    Timer* timer = new Timer(cb, when, interval);
    _loop->runInLoop(
            std::bind(&TimerQueue::addTimerInLoop, this, timer)
            );

    return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId) {
    _loop->runInLoop(
            std::bind(&TimerQueue::cancelInLoop, this, timerId)
            );
}

void TimerQueue::addTimerInLoop(Timer* timer) {
    _loop->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if (earliestChanged) {
        resetTimerfd(_timerfd, timer->expiration());
    }
}

void TimerQueue::cancelInLoop(TimerId timerId) {
    _loop->assertInLoopThread();
    ActiveTimer timer(timerId._value, timerId._seq);
    auto it = _activeTimers.find(timer);
    if (it != _activeTimers.end()) {
        size_t n = _timers.erase(Entry(it->first->expiration(), it->first));
        delete it->first;
        _activeTimers.erase(it);
    } else if (_callingExpiredTimers) {
        // 自cancel逻辑
        _cancelingTimers.insert(timer);
    }
}

void TimerQueue::handleRead() {
    _loop->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(_timerfd);

    std::vector<Entry> expired = getExpired(now);

    _callingExpiredTimers = true;
    _cancelingTimers.clear();
    for (auto it = expired.begin(); it != expired.end(); ++it) {
        it->second->run();
    }
    _callingExpiredTimers = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
    auto sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto it = _timers.lower_bound(sentry);
    std::copy(_timers.begin(), it, back_inserter(expired));
    _timers.erase(_timers.begin(), it);

    for (auto entry : expired) {
        ActiveTimer timer(entry.second, entry.second->sequence());
        auto n = _activeTimers.erase(timer);
    }

    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now) {
    Timestamp nextExpire;

    for (auto it = expired.cbegin(); it != expired.cend(); ++it) {
        ActiveTimer timer(it->second, it->second->sequence());
        if (it->second->repeat() && _cancelingTimers.find(timer) == _cancelingTimers.end()) {
            it->second->restart(now);
            insert(it->second);
        } else {
            delete it->second;
        }
    }

    if (!_timers.empty()) {
        nextExpire = _timers.begin()->second->expiration();
    }
    if (nextExpire.valid()) {
        resetTimerfd(_timerfd, nextExpire);
    }
}

bool TimerQueue::insert(Timer* timer) {
    _loop->assertInLoopThread();
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    auto it = _timers.begin();
    if (it == _timers.end() || when < it->first) {
        earliestChanged = true;
    }

    _timers.insert(std::make_pair(when, timer));
    _activeTimers.insert(ActiveTimer(timer, timer->sequence()));
    return earliestChanged;
}
