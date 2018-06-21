#ifndef MARIO_NET_TIMERID_H
#define MARIO_NET_TIMERID_H

namespace mario {

class Timer;

class TimerId {

public:
    TimerId(Timer* timer = NULL, int64_t seq = 0)
        : _value(timer)
        , _seq(seq)
    {}

    friend class TimerQueue;

private:
    Timer* _value;
    int64_t _seq;
};

}

#endif
