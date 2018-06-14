#ifndef MARIO_NET_TIMER_H
#define MARIO_NET_TIMER_H

#include "mario/base/Timestamp.h"
#include "mario/net/Callbacks.h"

namespace mario {

class Timer {

public:
    Timer(const TimerCallback& cb, Timestamp when, double interval)
        : _callback(cb)
        , _expiration(when)
        , _interval(interval)
        , _repeat(interval > 0.0)
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

    void restart(Timestamp now);

private:
    const TimerCallback _callback;
    Timestamp _expiration;
    const double _interval;
    const bool _repeat;

};

}

#endif
