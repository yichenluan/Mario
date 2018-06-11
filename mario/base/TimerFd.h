// Mac OS 没有提供 timerfd，所以自己利用 poll && pipe 封装了一个
// TODO. 阅读 brpc 源码，替换这个实现
#ifndef MARIO_BASE_TIMERFD_H
#define MARIO_BASE_TIMERFD_H

#include <atomic>

namespace mario {

class TimerFd {

public:
    
    TimerFd();
    ~TimerFd();

    int GetFd() const;
    void SetTime(const int timeoutMs);

private:
    void TimerFunc(int timeoutMs);

    int _pipeFd[2];
    std::atomic<bool> _isTiming;

};

}

#endif
