#ifndef MARIO_NET_ACCEPTOR_H
#define MARIO_NET_ACCEPTOR_H

#include <functional>

#include "mario/net/Channel.h"
#include "mario/net/Socket.h"

namespace mario {

class EventLoop;
class InetAddress;

class Acceptor {

public:
    typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr);

    void setNewConnectionCallback(const NewConnectionCallback& cb) {
        _newConnectionCallback = cb;
    }

    bool listenning() const {
        return _listenning;
    }

    void listen();

private:
    void handleRead();

    EventLoop* _loop;
    Socket _acceptSocket;
    Channel _acceptChannel;
    NewConnectionCallback _newConnectionCallback;
    bool _listenning;
};

}

#endif
