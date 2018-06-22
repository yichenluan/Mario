#ifndef MARIO_NET_EPOLLER_H
#define MARIO_NET_EPOLLER_H

#include <map>
#include <vector>

#include "mario/base/Timestamp.h"
#include "mario/net/EventLoop.h"

struct epoll_event;

namespace mario {

class Channel;

class EPoller {

public:
    typedef std::vector<Channel*> ChannelList;

    EPoller(EventLoop* loop);
    ~EPoller();

    Timestamp poll(int timeoutMs, ChannelList* activeChannels);

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread() {
        _loop->assertInLoopThread();
    }

private:
    static const int kInitEventListSize = 16;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    void update(int operation, Channel* channel);

    typedef std::vector<struct epoll_event> EventList;
    typedef std::map<int, Channel*> ChannelMap;

    EventLoop* _loop;
    int _epollfd;
    EventList _events;
    ChannelMap _channels;
};

}

#endif
