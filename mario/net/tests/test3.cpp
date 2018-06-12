#include "mario/net/Channel.h"
#include "mario/net/EventLoop.h"
#include "mario/base/TimerFd.h"
#include "mario/base/easylogging++.h"

#include <stdio.h>

mario::EventLoop* g_loop;

void timeout() {
    LOG(INFO) << "TimeOut!";
    // poll 是水平触发，如果不读fd的话，会立刻不停触发。
    g_loop->quit();
}

INITIALIZE_EASYLOGGINGPP

int main() {
    mario::EventLoop loop;
    g_loop = &loop;

    mario::TimerFd timeFd;
    timeFd.SetTime(5000);
    auto fd = timeFd.GetFd();

    //int timeFd = ::timefd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    mario::Channel channel(&loop, fd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    //struct itimerspec howlong;
    //bzero(&howlong, sizeof(howlong));

    //howlong.it_value.tv_sec = 5;
    //::timefd_settime(timeFd, 0, &howlong, NULL);

    loop.loop();

    //::close(timerfd);
}
