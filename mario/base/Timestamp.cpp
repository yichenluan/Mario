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

std::string Timestamp::toFormattedString() const {
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(_microSecondsSinceEpoch / kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(_microSecondsSinceEpoch % kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      microseconds);
  return buf;
}
