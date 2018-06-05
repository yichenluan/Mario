#include "mario/net/EventLoop.h"
#include "mario/base/Thread.h"
#include "mario/base/CurrentThread.h"
#include "mario/base/easylogging++.h"

#include <stdint.h>
#include <unistd.h>


void threadFunc() {
    LOG(INFO) << "threadFunc: pid = " << getpid() << ", tid = " << mario::CurrentThread::tid();
    mario::EventLoop loop;
    loop.loop();
}

INITIALIZE_EASYLOGGINGPP

int main() {
    LOG(INFO) << "main: pid = "<< getpid() << ", tid = " << mario::CurrentThread::tid();

    mario::EventLoop loop;

    mario::Thread thread(threadFunc);
    thread.start();

    loop.loop();

    pthread_exit(NULL);
}
