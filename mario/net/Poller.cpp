#include "mario/net/Poller.h"

#include "mario/net/Channel.h"
#include "mario/base/easylogging++.h"

#include <poll.h>

using namespace mario;

Poller::Poller(EventLoop* loop)
    : _loop(loop)
{}

Poller::~Poller() {}

Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels) {
    int numEvents = ::poll(_pollfds.data(), _pollfds.size(), timeoutMs);
    Timestamp now(Timestamp::now());
    if (numEvents > 0) {
        LOG(INFO) << numEvents << " events happended";
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        LOG(INFO) << "nothing happended";
    } else {
        LOG(FATAL) << "Poller::poll() fatal.";
    }
    return now;
}

void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    for (auto pfd_it = _pollfds.cbegin(); pfd_it != _pollfds.cend() && numEvents > 0; ++pfd_it) {
        if (pfd_it->revents > 0) {
            --numEvents;
            auto ch_it = _channels.find(pfd_it->fd);
            if (ch_it == _channels.end()) {
                LOG(FATAL) << "does't find channle";
            }

            Channel* channel = ch_it->second;
            channel->setRevents(pfd_it->revents);

            activeChannels->push_back(channel);
        }
    }
}

void Poller::updateChannel(Channel* channel) {
    assertInLoopThread();
    LOG(INFO) << "fd= "<< channel->fd() << ", events = " << channel->events();

    if (channel->index() < 0) {
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        _pollfds.push_back(pfd);

        int idx = static_cast<int>(_pollfds.size()) - 1;
        channel->setIndex(idx);
        _channels[pfd.fd] = channel;
    } else {
        int idx = channel->index();
        struct pollfd& pfd = _pollfds[idx];

        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        
        if (channel->isNoneEvent()) {
            pfd.fd = -1;
            pfd.fd = -channel->fd() - 1;
        }
    }
}

void Poller::removeChannel(Channel* channel) {
    assertInLoopThread();
    int idx = channel->index();
    const struct pollfd& pfd = _pollfds[idx];
    _channels.erase(channel->fd());

    if (idx == _pollfds.size() - 1) {
        _pollfds.pop_back();
    } else {
        int channelAtEnd = _pollfds.back().fd;
        iter_swap(_pollfds.begin()+idx, _pollfds.end()-1);
        if (channelAtEnd < 0) {
            channelAtEnd = -channelAtEnd - 1;
        }
        _channels[channelAtEnd]->setIndex(idx);
        _pollfds.pop_back();
    }
}
