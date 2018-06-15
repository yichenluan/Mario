#ifndef MARIO_NET_TCPCONNECTION_H
#define MARIO_NET_TCPCONNECTION_H

#include "mario/net/Callbacks.h"
#include "mario/net/InetAddress.h"

#include <memory>

namespace mario {

class Channel;
class EventLoop;
class Socket;

class TcpConnection: public std::enable_shared_from_this<TcpConnection> {

public:
    TcpConnection(EventLoop* loop,
            const std::string& name,
            int sockfd,
            const InetAddress& localAddr,
            const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const {
        return _loop;
    }

    const std::string& name() const {
        return _name;
    }

    const InetAddress& localAddress() {
        return _localAddr;
    }

    const InetAddress& peerAddress() {
        return _peerAddr;
    }

    bool connected() const {
        return _state == kConnected;
    }

    void setConnectionCallback(const ConnectionCallback& cb) {
        _connectionCallback = cb;
    }

    void setMessageCallback(const MessageCallback& cb) {
        _messageCallback = cb;
    }

    void connectEstablished();

private:
    enum  StateE {
        kConnecting,
        kConnected,
    };

    void setState(StateE s) {
        _state = s;
    }

    void handleRead();

    EventLoop* _loop;
    std::string _name;
    StateE _state;

    std::unique_ptr<Socket> _socket;
    std::unique_ptr<Channel> _channel;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
};

}

#endif
