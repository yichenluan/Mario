#include "mario/net/TcpServer.h"

#include "mario/base/easylogging++.h"
#include "mario/net/Acceptor.h"
#include "mario/net/EventLoop.h"
#include "mario/net/EventLoopThreadPool.h"
#include "mario/net/SocketsOps.h"

#include <functional>
#include <stdio.h>

using namespace mario;
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
    : _loop(loop)
    , _name(listenAddr.toHostPort())
    , _acceptor(new Acceptor(loop, listenAddr))
    , _threadPool(new EventLoopThreadPool(loop))
    , _started(false)
    , _nextConnId(1)
{
    _acceptor->setNewConnectionCallback(
            std::bind(&TcpServer::newConnection, this, _1, _2)
            );
}

TcpServer::~TcpServer() {}

void TcpServer::setThreadNum(int numThreads) {
    _threadPool->setThreadNum(numThreads);
}

void TcpServer::start() {
    if (!_started) {
        _started = true;
        _threadPool->start();
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

    EventLoop* ioLoop = _threadPool->getNextLoop();
    TcpConnectionPtr conn(
            new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr)
            );
    _connections[connName] = conn;
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setCloseCallback(
            std::bind(&TcpServer::removeConnection, this, _1)
            );
    //conn->connectEstablished();
    ioLoop->runInLoop(
            std::bind(&TcpConnection::connectEstablished, conn)
            );
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
    _loop->runInLoop(
            std::bind(&TcpServer::removeConnectionInLoop, this, conn)
            );
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
    _loop->assertInLoopThread();
    LOG(INFO) << "TcpServer::removeConnectionInLoop [" << _name << "] - connection " << conn->name();
    _connections.erase(conn->name());
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
            std::bind(&TcpConnection::connectDestroyed, conn)
            );
}
