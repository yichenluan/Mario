#include "mario/net/TcpConnection.h"

#include "mario/base/easylogging++.h"
#include "mario/net/Channel.h"
#include "mario/net/EventLoop.h"
#include "mario/net/Socket.h"

#include <functional>
#include <stdio.h>
#include <unistd.h>

using namespace mario;

TcpConnection::TcpConnection(EventLoop* loop,
        const std::string& nameArg,
        int sockfd,
        const InetAddress& localAddr,
        const InetAddress& peerAddr)
    : _loop(loop)
    , _name(nameArg)
    , _state(kConnecting)
    , _socket(new Socket(sockfd))
    , _channel(new Channel(loop, sockfd))
    , _localAddr(localAddr)
    , _peerAddr(peerAddr)
{
    LOG(INFO) << "TcpConnection::ctor name: " << _name << ", fd: " << _socket->fd();
    _channel->setReadCallback(
            std::bind(&TcpConnection::handleRead, this)
            );
}

TcpConnection::~TcpConnection() {
    LOG(INFO) << "TcpConnection::dtor name: " << _name << ", fd: " << _socket->fd();
}

void TcpConnection::connectEstablished() {
    _loop->assertInLoopThread();
    setState(kConnected);
    _channel->enableReading();

    _connectionCallback(shared_from_this());
}

void TcpConnection::handleRead() {
    char buf[65536];
    ssize_t n = ::read(_channel->fd(), buf, sizeof(buf));
    _messageCallback(shared_from_this(), buf, n);
}
