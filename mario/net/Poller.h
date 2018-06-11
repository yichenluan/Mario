#ifndef MARIO_NET_POLLER_H
#define MARIO_NET_POLLER_H

#include "mario/net/EventLoop.h"

#include <map>
#include <vector>

struct pollfd;

namespace mario {


class Channel;

class Poller {

public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop);
    ~Poller();

    void poll(int timeoutMs, ChannelList* activeChannels);

    void updateChannel(Channel* channel);

    void assertInLoopThread() {
        _loop->assertInLoopThread();
    }

private:

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int, Channel*> ChannelMap;

    EventLoop* _loop;
    PollFdList _pollfds;
    ChannelMap _channels;

};

}

#endif
