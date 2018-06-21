#include "mario/net/EventLoop.h"
#include "mario/base/easylogging++.h"

#include <functional>
#include <stdint.h>
#include <unistd.h>

mario::EventLoop* g_loop;
int cnt = 0;

void printTid() {
    LOG(INFO) << "pid = " << getpid() << ", tid = " << mario::CurrentThread::tid();
}

void print(const std::string msg) {
    LOG(INFO) << "msg: " << msg;
    if (++cnt == 20) {
        g_loop->quit();
    }
}

mario::TimerId toCancel;

void cancelSelf() {
    LOG(INFO) << "cancelSelf()";
    g_loop->cancel(toCancel);
}

INITIALIZE_EASYLOGGINGPP

int main() {
    printTid();
    mario::EventLoop loop;
    g_loop = &loop;

    LOG(INFO) << "main beign";

    loop.runAfter(1, std::bind(print, "once 1"));
    loop.runAfter(1.5, std::bind(print, "once 1.5"));
    loop.runAfter(2.5, std::bind(print, "once 2.5"));
    loop.runAfter(3.5, std::bind(print, "once 3.5"));
    //loop.runEvery(2, std::bind(print, "every 2"));
    mario::TimerId  t = loop.runEvery(2, std::bind(print, "every 2"));
    loop.runEvery(3, std::bind(print, "every 3"));

    loop.runAfter(10, std::bind(&mario::EventLoop::cancel, &loop, t));

    toCancel = loop.runEvery(5, cancelSelf);

    loop.loop();

    LOG(INFO) << "main stop.";
    sleep(1);
}
