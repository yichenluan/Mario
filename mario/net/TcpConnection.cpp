#include "mario/net/TcpConnection.h"

#include "mario/base/easylogging++.h"
#include "mario/net/Channel.h"
#include "mario/net/EventLoop.h"
#include "mario/net/Socket.h"
#include "mario/net/SocketsOps.h"

#include <functional>
#include <stdio.h>
#include <unistd.h>

using namespace mario;
using namespace std::placeholders;

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
            std::bind(&TcpConnection::handleRead, this, _1)
        );
    _channel->setWriteCallback(
            std::bind(&TcpConnection::handleWrite, this)
        );
    _channel->setCloseCallback(
            std::bind(&TcpConnection::handleClose, this)
        );
    _channel->setErrorCallback(
            std::bind(&TcpConnection::handleError, this)
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

void TcpConnection::send(const std::string& message) {
    if (_state == kConnected) {
        if (_loop->isInLoopThread()) {
            sendInLoop(message);
        } else {
            _loop->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
        }
    }
}

void TcpConnection::sendInLoop(const std::string& message) {
    _loop->assertInLoopThread();
    ssize_t nwrote = 0;
    if (!_channel->isWriting() && _outputBuffer.readableBytes() == 0) {
        nwrote = ::write(_channel->fd(), message.data(), message.size());
        if (nwrote >= 0) {
            if (nwrote < message.size()) {
                LOG(INFO) << "more data to write";
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG(FATAL) << "TcpConnection::sendInLoop";
            }
        }
    }

    if (nwrote < message.size()) {
        _outputBuffer.append(message.data() + nwrote, message.size() - nwrote);
        if (!_channel->isWriting()) {
            _channel->enableWriting();
        }
    }
}

void TcpConnection::shutdown() {
    if (_state == kConnected) {
        setState(kDisconnecting);
        _loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop() {
    _loop->assertInLoopThread();
    if (!_channel->isWriting()) {
        _socket->shutdownWrite();
    }
}

void TcpConnection::connectDestroyed() {
    _loop->assertInLoopThread();
    setState(kDisconnected);
    _channel->disableAll();
    _connectionCallback(shared_from_this());

    _loop->removeChannel(_channel.get());
}

void TcpConnection::handleRead(Timestamp receiveTime) {
    int savedErrno = 0;
    ssize_t n = _inputBuffer.readFd(_channel->fd(), &savedErrno);
    if (n > 0) {
        _messageCallback(shared_from_this(), &_inputBuffer, receiveTime);
    } else if (n == 0) {
        handleClose();
    } else {
        handleError();
    }
}

//void TcpConnection::handleRead() {
    //char buf[65536];
    //ssize_t n = ::read(_channel->fd(), buf, sizeof(buf));
    //if (n > 0) {
        //_messageCallback(shared_from_this(), buf, n);
    //} else if (n == 0) {
        //handleClose();
    //} else {
        //handleError();
    //}
//}

void TcpConnection::handleWrite() {
    _loop->assertInLoopThread();
    if (_channel->isWriting()) {
        ssize_t n = ::write(_channel->fd(), _outputBuffer.peek(),
                _outputBuffer.readableBytes());
        if (n > 0) {
            _outputBuffer.retrieve(n);
            if (_outputBuffer.readableBytes() == 0) {
                _channel->disableWriting();
                if (_state == kDisconnecting) {
                    shutdownInLoop();
                }
            } else {
                LOG(INFO) << "more data to write";
            }
        } else {
            LOG(FATAL) << "TcpConnection::handleWrite";
        }
    } else {
        LOG(INFO) << "Connection is down";
    }
}

void TcpConnection::handleClose() {
    _loop->assertInLoopThread();
    LOG(INFO) << "TcpConnection::handleClose state : " << _state;

    _channel->disableAll();
    _closeCallback(shared_from_this());
}

void TcpConnection::handleError() {
    int err = sockets::getSocketError(_channel->fd());
    LOG(WARNING) << "TcpConnection::handleError() :[" << _name << "] - SO_ERROR = " << err;
}
