#ifndef MARIO_NET_CONNECTOR_H
#define MARIO_NET_CONNECTOR_H

#include "mario/net/InetAddress.h"
#include "TimerId.h"

#include <memory>
#include <functional>

namespace mario {

class Channel;
class EventLoop;

class Connector {

public:
    typedef std::function<void (int sockfd)> NewConnectionCallback;

    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb) {
        _newConnectionCallback = cb;
    }

    void start();
    void restart();
    void stop();

    const InetAddress& serverAddress() const {
        return _serverAddr;
    }

private:
    enum States {
        kDisconnected,
        kConnecting,
        kConnected
    };

    static const int kMaxRetryDelayMs = 30 * 1000;
    static const int kInitRetryDelayMs = 500;

    void setState(States s) {
        _state = s;
    }

    void startInLoop();
    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();

    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

    EventLoop* _loop;
    InetAddress _serverAddr;
    bool _connect;
    States _state;
    std::unique_ptr<Channel> _channel;
    NewConnectionCallback _newConnectionCallback;
    int _retryDelayMs;
    TimerId _timerId;
};

typedef std::shared_ptr<Connector> ConnectorPtr;

}

#endif
