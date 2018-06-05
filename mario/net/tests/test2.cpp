#include "mario/net/EventLoop.h"
#include "mario/base/Thread.h"
#include "mario/base/easylogging++.h"

mario::EventLoop* g_loop;

void threadFunc() {
    g_loop->loop();
}

INITIALIZE_EASYLOGGINGPP

int main() {
    mario::EventLoop loop;
    mario::EventLoop loop2;
    loop.loop();
    loop2.loop();
    //g_loop = &loop;
    //mario::Thread t(threadFunc);
    //t.start();
    //t.join();
}
