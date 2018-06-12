#ifndef MARIO_NET_EVENTLOOP_H
#define MARIO_NET_EVENTLOOP_H

#include "mario/base/Thread.h"
#include "mario/base/CurrentThread.h"

#include <vector>

namespace mario {

class Channel;
class Poller;

class EventLoop {

public:

	EventLoop();
	~EventLoop();

	// EventLoop is noncopyable.
	// TODO. Learn about copy construction
	EventLoop(const EventLoop&) = delete;
	EventLoop& operator = (const EventLoop&) = delete;

	void loop();

    void quit();

    void updateChannel(Channel* channel);

	void assertInLoopThread() {
		if (!isInLoopThread()) {
			abortNotInLoopThread();
		}
	}

	bool isInLoopThread() const {
		return _threadId == CurrentThread::tid();
	}

private:
    typedef std::vector<Channel*> ChannelList;

	void abortNotInLoopThread();

	bool _looping;
    bool _quit;
	const pid_t _threadId;
    // TODO. Learn more about smart ptr.
    std::unique_ptr<Poller> _poller;
    ChannelList _activeChannels;
};


}

#endif
