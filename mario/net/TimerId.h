#ifndef MARIO_NET_TIMERID_H
#define MARIO_NET_TIMERID_H

namespace mario {

class Timer;

class TimerId {

public:
    explicit TimerId(Timer* timer)
        : _value(timer)
    {}

private:
    Timer* _value;
};

}

#endif
