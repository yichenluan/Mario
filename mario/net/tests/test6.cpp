#include "mario/net/Channel.h"
#include "mario/net/EventLoop.h"
#include "mario/net/EventLoopThread.h"
#include "mario/base/CurrentThread.h"
#include "mario/base/easylogging++.h"
#include "mario/base/TimerFd.h"

#include <stdio.h>
#include <unistd.h>

void runInThread() {
    LOG(INFO) << "runInThread(): pid = " << getpid() << ", tid = " << mario::CurrentThread::tid();
}


INITIALIZE_EASYLOGGINGPP

int main() {
    LOG(INFO) << "main: pid = "<< getpid() << ", tid = " << mario::CurrentThread::tid();

    mario::EventLoopThread loopThread;
    mario::EventLoop* loop = loopThread.startLoop();
    
    mario::TimerFd timeFd;
    timeFd.SetTime(1000);

    auto fd = timeFd.GetFd();
    mario::Channel channel(loop, fd);
    channel.setReadCallback(runInThread);
    channel.enableReading();
    sleep(15);
}
