#ifndef MARIO_NET_TIMER_H
#define MARIO_NET_TIMER_H

#include "mario/base/Timestamp.h"
#include "mario/net/Callbacks.h"
#include "mario/base/Atomic.h"

namespace mario {

class Timer {

public:
    Timer(const TimerCallback& cb, Timestamp when, double interval)
        : _callback(cb)
        , _expiration(when)
        , _interval(interval)
        , _repeat(interval > 0.0)
        , _sequence(_s_numCreated.incrementAndGet())
    {}

    void run() const {
        _callback();
    }

    Timestamp expiration() const {
        return _expiration;
    }

    bool repeat() const {
        return _repeat;
    }

    int64_t sequence() const {
        return _sequence;
    }

    void restart(Timestamp now);

private:
    const TimerCallback _callback;
    Timestamp _expiration;
    const double _interval;
    const bool _repeat;
    const int64_t _sequence;

    static AtomicInt64 _s_numCreated;
};

}

#endif
