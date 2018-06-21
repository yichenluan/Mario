#include "mario/net/Connector.h"

#include "mario/net/Channel.h"
#include "mario/net/EventLoop.h"
#include "mario/net/SocketsOps.h"
#include "mario/base/easylogging++.h"

#include <errno.h>

using namespace mario;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
    : _loop(loop)
    , _serverAddr(serverAddr)
    , _connect(false)
    , _state(kDisconnected)
    , _retryDelayMs(kInitRetryDelayMs) 
{}

Connector::~Connector() {
    _loop->cancel(_timerId);
}

void Connector::start() {
    _connect = true;
    _loop->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop() {
    _loop->assertInLoopThread();
    if (_connect) {
        connect();
    }
}

void Connector::connect() {
    int sockfd = sockets::createNonblockingOrDie();
    int ret = sockets::connect(sockfd, _serverAddr.getSockAddrInet());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
            break;
        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;
        default:
            sockets::close(sockfd);
            LOG(FATAL) << "connect error";
            break;
    }
}

void Connector::restart() {
    _loop->assertInLoopThread();
    setState(kDisconnected);
    _retryDelayMs = kInitRetryDelayMs;
    _connect = true;
    startInLoop();
}

void Connector::stop() {
    _connect = false;
    _loop->cancel(_timerId);
}

void Connector::connecting(int sockfd) {
    setState(kConnecting);
    _channel.reset(new Channel(_loop, sockfd));
    _channel->setWriteCallback(
            std::bind(&Connector::handleWrite, this)
            );
    _channel->setErrorCallback(
            std::bind(&Connector::handleError, this)
            );
    _channel->enableWriting();
}

int Connector::removeAndResetChannel() {
    _channel->disableAll();
    _loop->removeChannel(_channel.get());
    int sockfd = _channel->fd();
    _loop->queueInLoop(
            std::bind(&Connector::resetChannel, this)
            );
    return sockfd;
}

void Connector::resetChannel() {
    _channel.reset();
}

void Connector::handleWrite() {
    LOG(INFO) << "Connector::handleWrite";
    if (_state == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if (err) {
            LOG(WARNING) << "Connector::handleWrite - ERROR: " << err;
            retry(sockfd);
        } else {
            setState(kConnected);
            if (_connect) {
                _newConnectionCallback(sockfd);
            } else {
                sockets::close(sockfd);
            }
        }
    }
}

void Connector::handleError() {
    LOG(WARNING) << "Connector::handleError";
    int sockfd = removeAndResetChannel();
    retry(sockfd);
}

void Connector::retry(int sockfd) {
    sockets::close(sockfd);
    setState(kDisconnected);
    if (_connect) {
        LOG(INFO) << "Connector::retry in " << _retryDelayMs << " milliseconds";
        _timerId = _loop->runAfter(_retryDelayMs / 1000.0,
                std::bind(&Connector::startInLoop, this));
        _retryDelayMs = std::min(_retryDelayMs * 2, kMaxRetryDelayMs);
    }
}
