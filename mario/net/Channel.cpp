#include "mario/net/Channel.h"

#include "mario/net/EventLoop.h"
#include "mario/base/easylogging++.h"

#include <poll.h>

using namespace mario;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;


Channel::Channel(EventLoop* loop, int fdArg)
    : _loop(loop),
     _fd(fdArg),
     _events(0),
     _revents(0),
     _index(-1),
     _eventHandling(false)
{}

Channel::~Channel() {
    if (_eventHandling) {
        LOG(FATAL) << "~Channel Fatal, _eventHandling";
    }
}

void Channel::update() {
    _loop->updateChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime) {
    _eventHandling = true;
    // TODO. What's POLL EVENT
    if (_revents & POLLNVAL) {
        LOG(WARNING) << "Channel::handleEvent() POLLNVAL";
    }

    if ((_revents & POLLHUP) && !(_revents & POLLIN)) {
        LOG(WARNING) << "Channel::handleEvent() POLLHUP";
        if (_closeCallback) {
            _closeCallback();
        }
    }

    if (_revents & (POLLERR | POLLNVAL)) {
        if (_errorCallback) {
            _errorCallback();
        }
    }
    if (_revents & (POLLIN | POLLPRI | POLLHUP)) {
        if (_readCallback) {
            _readCallback(receiveTime);
        }
    }

    if (_revents & POLLOUT) {
        if (_writeCallback) {
            _writeCallback();
        }
    }
    _eventHandling = false;
}
