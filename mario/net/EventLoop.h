#ifndef MARIO_NET_EVENTLOOP_H
#define MARIO_NET_EVENTLOOP_H

#include "mario/base/Thread.h"
#include "mario/base/CurrentThread.h"

namespace mario {

class EventLoop {

public:

	EventLoop();
	~EventLoop();

	// EventLoop is noncopyable.
	// TODO. Learn about copy construction
	EventLoop(const EventLoop&) = delete;
	EventLoop& operator = (const EventLoop&) = delete;

	void loop();

	void assertInLoopThread() {
		if (!isInLoopThread()) {
			abortNotInLoopThread();
		}
	}

	bool isInLoopThread() const {
		return _threadId == CurrentThread::tid();
	}

private:

	void abortNotInLoopThread();

	bool _looping;
	const pid_t _threadId;
};


}

#endif
