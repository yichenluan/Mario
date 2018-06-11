#include "mario/base/TimerFd.h"

#include "mario/base/Thread.h"

#include "mario/base/easylogging++.h"

#include <unistd.h>
#include <poll.h>

using namespace mario;

TimerFd::TimerFd()
    : _isTiming(false)
{
    if(::pipe(_pipeFd) == -1) {
        LOG(FATAL) << "TimerFd Init Failed.";
    }
}

TimerFd::~TimerFd() {
    ::close(_pipeFd[0]);
    ::close(_pipeFd[1]);
}

void TimerFd::TimerFunc(int timeoutMs) {
    ::poll(NULL, 0, timeoutMs);
    ::write(_pipeFd[1], " ", 1);
    _isTiming = false;
}

int TimerFd::GetFd() const {
    return _pipeFd[0];
}

void TimerFd::SetTime(const int timeoutMs) {
    if (_isTiming) {
        LOG(FATAL) << "Timer is running.";
        return;
    }

    _isTiming = true;

    Thread timer(std::bind(&TimerFd::TimerFunc, this, timeoutMs));
    timer.start();
}
