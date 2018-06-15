#include "mario/net/TcpServer.h"

#include "mario/base/easylogging++.h"
#include "mario/net/Acceptor.h"
#include "mario/net/EventLoop.h"
#include "mario/net/SocketsOps.h"

#include <functional>
#include <stdio.h>

using namespace mario;
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
    : _loop(loop)
    , _name(listenAddr.toHostPort())
    , _acceptor(new Acceptor(loop, listenAddr))
    , _started(false)
    , _nextConnId(1)
{
    _acceptor->setNewConnectionCallback(
            std::bind(&TcpServer::newConnection, this, _1, _2)
            );
}

TcpServer::~TcpServer() {}

void TcpServer::start() {
    if (!_started) {
        _started = true;
    }

    if (!_acceptor->listenning()) {
        _loop->runInLoop(
                std::bind(&Acceptor::listen, _acceptor.get())
            );
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) {
    _loop->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", _nextConnId);
    ++_nextConnId;
    std::string connName = _name + buf;

    LOG(INFO) << "TcpServer::newConnection [" << _name << "] - new connection [" << connName << "] from " << peerAddr.toHostPort();

    InetAddress localAddr(sockets::getLocalAddr(sockfd));

    TcpConnectionPtr conn(
            new TcpConnection(_loop, connName, sockfd, localAddr, peerAddr)
            );
    _connections[connName] = conn;
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->connectEstablished();
}
