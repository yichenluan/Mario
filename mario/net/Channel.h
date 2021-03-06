#ifndef MARIO_NET_CHANNEL_H
#define MARIO_NET_CHANNEL_H

#include <functional>

#include "mario/base/Timestamp.h"

namespace mario {

class EventLoop;

class Channel {

public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void (Timestamp)> ReadEventCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTime);

    void setReadCallback(const ReadEventCallback& cb) {
        _readCallback = cb;
    }

    void setWriteCallback(const EventCallback& cb) {
        _writeCallback = cb;
    }

    void setErrorCallback(const EventCallback& cb) {
        _errorCallback = cb;
    }

    void setCloseCallback(const EventCallback& cb) {
        _closeCallback = cb;
    }

    int fd() const {
        return _fd;
    }

    int events() const {
        return _events;
    }

    void setRevents(int revt) {
        _revents = revt;
    }

    bool isNoneEvent() const {
        return _events == kNoneEvent;
    }

    void enableReading() {
        _events |= kReadEvent;
        update();
    }

    void enableWriting() {
        _events |= kWriteEvent;
        update();
    }

    void disableWriting() {
        _events &= ~kWriteEvent;
        update();
    }

    void disableAll() {
        _events = kNoneEvent;
        update();
    }

    bool isWriting() const {
        return _events & kWriteEvent;
    }

    // For Poller
    int index() {
        return _index;
    }

    void setIndex(int idx) {
        _index = idx;
    }

    EventLoop* ownerLoop() {
        return _loop;
    }

private:

    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* _loop;
    const int _fd;
    int _events;
    int _revents;
    int _index;

    bool _eventHandling;

    ReadEventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _errorCallback;
    EventCallback _closeCallback;

};

}

#endif
