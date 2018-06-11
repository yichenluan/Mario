#include "mario/net/EventLoop.h"

#include "mario/net/Channel.h"
#include "mario/net/Poller.h"
#include "mario/base/easylogging++.h"

// TODO. Select/Poll/Epoll Diff And Why.
//#include <poll.h>

using namespace mario;

__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

EventLoop::EventLoop() 
	: _looping(false)
    , _poller(new Poller(this))
	, _threadId(CurrentThread::tid()) {

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
    _quit = false;

    while (!_quit) {
        _activeChannels.clear();
        _poller->poll(kPollTimeMs, &_activeChannels);

        for (auto channel_p : _activeChannels) {
            channel_p->handleEvent();
        }
    }

	LOG(INFO) << "EventLoop" << this << " stop looping";
	_looping = false;
}

void EventLoop::quit() {
    _quit = true;
}

void EventLoop::updateChannel(Channel* channel) {
    assertInLoopThread();
    _poller->updateChannel(channel);
}

void EventLoop::abortNotInLoopThread() {
	LOG(FATAL) << "EventLoop::abortNotInLoopThread - EventLoop " << this
			  << " was created in _threadId = " << _threadId
			  << ", current thread id = " << CurrentThread::tid();
}
