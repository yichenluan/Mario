#ifndef MARIO_NET_TCPSERVER_H
#define MARIO_NET_TCPSERVER_H

#include "mario/net/Callbacks.h"
#include "mario/net/TcpConnection.h"

#include <map>
#include <memory>

namespace mario {

class Acceptor;
class EventLoop;

class TcpServer {

public:
    TcpServer(EventLoop* loop, const InetAddress& listenAddr);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb) {
        _connectionCallback = cb;
    }

    void setMessageCallback(const MessageCallback& cb) {
        _messageCallback = cb;
    }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);

    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    EventLoop* _loop;
    const std::string _name;
    std::unique_ptr<Acceptor> _acceptor;
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    bool _started;
    int _nextConnId;
    ConnectionMap _connections;
};

}

#endif
