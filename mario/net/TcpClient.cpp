#include "mario/net/TcpClient.h"

#include "mario/net/Connector.h"
#include "mario/net/EventLoop.h"
#include "mario/net/SocketsOps.h"
#include "mario/base/easylogging++.h"

#include <functional>

#include <stdio.h>

using namespace mario;
using namespace std::placeholders;

namespace mario {

namespace detail {

void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn) {
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void removeConnector(const ConnectorPtr& connector) {

}

}

}

TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr)
    : _loop(loop)
    , _connector(new Connector(loop, serverAddr))
    , _retry(false)
    , _connect(true)
    , _nextConnId(1)
{
    _connector->setNewConnectionCallback(
            std::bind(&TcpClient::newConnection, this, _1)
            );
}

TcpClient::~TcpClient() {
    TcpConnectionPtr conn;
    {
        MutexLockGuard lock(_mutex);
        conn = _connection;
    }

    if (conn) {
        CloseCallback cb = std::bind(&detail::removeConnection, _loop, _1);
        _loop->runInLoop(
                std::bind(&TcpConnection::setCloseCallback, conn, cb)
                );
    } else {
        _connector->stop();
        _loop->runAfter(1, std::bind(&detail::removeConnector, _connector));
    }
}

void TcpClient::connect() {
    LOG(INFO) << "TcpClient::connect[" << this << "] - connecting to "
        << _connector->serverAddress().toHostPort();
    _connect = true;
    _connector->start();
}

void TcpClient::disconnect() {
    _connect = false;
    {
        MutexLockGuard lock(_mutex);
        if (_connection) {
            _connection->shutdown();
        }
    }
} 

void TcpClient::stop() {
    _connect = false;
    _connector->stop();
}

void TcpClient::newConnection(int sockfd) {
    _loop->assertInLoopThread();
    InetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toHostPort().c_str(), _nextConnId);
    ++_nextConnId;
    std::string connName = buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));

    TcpConnectionPtr conn(
            new TcpConnection(_loop, connName, sockfd, localAddr, peerAddr)
            );
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setCloseCallback(
            std::bind(&TcpClient::removeConnection, this, _1)
            );
    {
        MutexLockGuard lock(_mutex);
        _connection = conn;
    }
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn) {
    _loop->assertInLoopThread();
    {
        MutexLockGuard lock(_mutex);
        _connection.reset();
    }
    _loop->queueInLoop(
            std::bind(&TcpConnection::connectDestroyed, conn)
            );
    if (_retry && _connect) {
        LOG(INFO) << "TcpClient::connect [" << this << "] - Reconnecting to "
            << _connector->serverAddress().toHostPort();
        _connector->restart();
    }
}
