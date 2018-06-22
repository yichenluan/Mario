#ifndef MARIO_NET_TCPCLIENT_H
#define MARIO_NET_TCPCLIENT_H

#include "mario/base/Mutex.h"
#include "mario/net/TcpConnection.h"

namespace mario {

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient {

public:
    TcpClient(EventLoop* loop, const InetAddress& serverAddr);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() const {
        MutexLockGuard lock(_mutex);
        return _connection;
    }

    bool retry() const;

    void enableRetry() {
        _retry = true;
    }

    void setConnectionCallback(const ConnectionCallback& cb) {
        _connectionCallback = cb;
    }

    void setMessageCallback(const MessageCallback& cb) {
        _messageCallback = cb;
    }

private:
    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* _loop;
    ConnectorPtr _connector;
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;

    bool _retry;
    bool _connect;

    int _nextConnId;

    mutable MutexLock _mutex;

    TcpConnectionPtr _connection;
};

}

#endif
