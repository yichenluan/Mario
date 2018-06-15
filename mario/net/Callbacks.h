#ifndef MARIO_NET_CALLBACKS_h
#define MARIO_NET_CALLBACKS_h

#include <functional>
#include <memory>

namespace mario {

typedef std::function<void()> TimerCallback;

class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&,
        const char* data, ssize_t len)> MessageCallback;

}

#endif
