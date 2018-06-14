#include "mario/net/Timer.h"

using namespace mario;

void Timer::restart(Timestamp now) {
    if (_repeat) {
        _expiration = addTime(now, _interval);
    } else {
        _expiration  = Timestamp::invalid();
    }
}
