#ifndef MARIO_NET_TIMERQUEUE_H
#define MARIO_NET_TIMERQUEUE_H

#include <set>
#include <vector>

#include "mario/base/Timestamp.h"
#include "mario/base/Mutex.h"
#include "mario/net/Callbacks.h"
#include "mario/net/Channel.h"

namespace mario {

class EventLoop;
class Timer;
class TimerId;

class TimerQueue {

public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);

    void cancel(TimerId timerId);

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);

    void handleRead();

    std::vector<Entry> getExpired(Timestamp now);

    void reset(const std::vector<Entry>& expired, Timestamp now);

    bool insert(Timer* timer);

    EventLoop* _loop;
    const int _timerfd;
    Channel _timerfdChannel;

    TimerList _timers;

    bool _callingExpiredTimers;
    ActiveTimerSet _activeTimers;
    ActiveTimerSet _cancelingTimers;

};

}

#endif
