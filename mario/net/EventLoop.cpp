#include "EventLoop.h"

#include "mario/base/easylogging++.h"

// TODO. Select/Poll/Epoll Diff And Why.
#include <poll.h>

using namespace mario;

__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop() 
	: _looping(false),
	  _threadId(CurrentThread::tid()) {

	LOG(INFO) << "EventLoop created" << this << " in thread " << _threadId;
	if (t_loopInThisThread) {
		LOG(FATAL) << "Another EventLoop " << t_loopInThisThread
				  << " exists in this thread " << _threadId;
	} else {
		t_loopInThisThread = this;
	}
}

EventLoop::~EventLoop() {
	t_loopInThisThread = NULL;
}

void EventLoop::loop() {
	assertInLoopThread();
	_looping = true;

	::poll(NULL, 0, 5*1000);

	LOG(INFO) << "EventLoop" << this << " stop looping";
	_looping = false;
}

void EventLoop::abortNotInLoopThread() {
	LOG(FATAL) << "EventLoop::abortNotInLoopThread - EventLoop " << this
			  << " was created in _threadId = " << _threadId
			  << ", current thread id = " << CurrentThread::tid();
}
