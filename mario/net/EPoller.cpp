#include "mario/net/EPoller.h"

#include "mario/net/Channel.h"
#include "mario/base/easylogging++.h"

#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

using namespace mario;

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPoller::EPoller(EventLoop* loop)
    : _loop(loop)
    , _epollfd(::epoll_create1(EPOLL_CLOEXEC))
    , _events(kInitEventListSize)
{
    if (_epollfd < 0) {
        LOG(FATAL) << "EPoller::EPoller";
    }
}

EPoller::~EPoller() {
    ::close(_epollfd);
}

Timestamp EPoller::poll(int timeoutMs, ChannelList* activeChannels) {
    int numEvents = ::epoll_wait(_epollfd,
            _events.data(),
            _events.size(),
            timeoutMs);
    Timestamp now(Timestamp::now());
    if (numEvents > 0) {
        LOG(INFO) << numEvents << "events happended";
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == _events.size()) {
            _events.resize(_events.size() * 2);
        }
    } else if (numEvents == 0) {
        LOG(INFO) << "nothing happended";
    } else {
        LOG(FATAL) << "EPoller::poll()";
    }
    return now;
}

void EPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    for (int i = 0; i < numEvents; ++i) {
        Channel* channel = static_cast<Channel*>(_events[i].data.ptr);
        channel->setRevents(_events[i].events);
        activeChannels->push_back(channel);
    }
}

void EPoller::updateChannel(Channel* channel) {
    assertInLoopThread();
    LOG(INFO) << "fd= "<< channel->fd() << ", events = " << channel->events();
    const int index = channel->index();
    if (index == kNew || index == kDeleted) {
        int fd = channel->fd();
        if (index == kNew) {
            _channels[fd] = channel;
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        int fd = channel->fd();
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoller::removeChannel(Channel* channel) {
    assertInLoopThread();
    int fd = channel->fd();
    int index = channel->index();
    _channels.erase(fd);
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void EPoller::update(int operation, Channel* channel) {
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(_epollfd, operation, fd, &event) < 0) {
        LOG(FATAL) << "epoll_ctl op = " << operation << " fd = " << fd;
    }
}
