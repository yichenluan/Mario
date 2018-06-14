#ifndef MARIO_NET_EVENTLOOP_H
#define MARIO_NET_EVENTLOOP_H

#include "mario/base/Thread.h"
#include "mario/base/CurrentThread.h"
#include "mario/base/Timestamp.h"
#include "mario/net/TimerId.h"
#include "mario/net/Callbacks.h"

#include <vector>
#include <memory>

namespace mario {

class Channel;
class Poller;
class TimerQueue;

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

    Timestamp pollReturnTime() const {
        return _pollReturnTime;
    }

    TimerId runAt(const Timestamp& time, const TimerCallback& cb);

    TimerId runAfter(double delay, const TimerCallback& cb);

    TimerId runEvery(double interval, const TimerCallback& cb);

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
    Timestamp _pollReturnTime;
    // TODO. Learn more about smart ptr.
    std::unique_ptr<Poller> _poller;
    std::unique_ptr<TimerQueue> _timerQueue;
    ChannelList _activeChannels;
};


}

#endif
