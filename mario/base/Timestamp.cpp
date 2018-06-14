#include "mario/base/Timestamp.h"

#include <sys/time.h>
#include <stdio.h>

using namespace mario;

Timestamp::Timestamp()
    : _microSecondsSinceEpoch(0)
{}

Timestamp::Timestamp(int64_t microseconds)
    : _microSecondsSinceEpoch(microseconds)
{}

Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

Timestamp Timestamp::invalid() {
    return Timestamp();
}
