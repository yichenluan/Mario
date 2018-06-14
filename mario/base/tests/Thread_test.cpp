#include "mario/base/Thread.h"
#include "mario/base/CurrentThread.h"

#include "mario/base/easylogging++.h"

#include <string>
#include <functional>
#include <stdint.h>
#include <unistd.h>
#include <iostream>

void mysleep(int seconds) {
	timespec t = { seconds, 0 };
	nanosleep(&t, NULL);
}

void threadFunc1() {
    //LOG(INFO) <<  "[thread1] pid = " << getpid() << ", tid = " << mario::CurrentThread::tid();
	for (int i = 1; i < 5; ++i) {
		//LOG(INFO) << "I am thread 1";
        std::cout << "thread 1" << std::endl;
		mysleep(1);
	}
}

void threadFunc2() {
    //LOG(INFO) <<  "[thread2] pid = " << getpid() << ", tid = " << mario::CurrentThread::tid();
	for (int i = 1; i < 5; ++i) {
		//LOG(INFO) << "I am thread 2";
        std::cout << "thread 2" << std::endl;
		mysleep(1);
	}
}

INITIALIZE_EASYLOGGINGPP

int main() {
    LOG(INFO)  <<  "[main] pid = " << getpid() << ", tid = " << mario::CurrentThread::tid();
	mario::Thread t1(threadFunc1);
	t1.start();

	mario::Thread t2(threadFunc2);
	t2.start();

    LOG(INFO) << "t1.tid = " << t1.tid();
    LOG(INFO) << "t2.tid = " << t2.tid();

	
	t1.join();
	t2.join();

	LOG(INFO) << "all thread ends";
}
