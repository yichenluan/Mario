#ifndef MARIO_NET_CALLBACKS_h
#define MARIO_NET_CALLBACKS_h

#include "mario/base/Timestamp.h"

#include <functional>
#include <memory>

namespace mario {

typedef std::function<void()> TimerCallback;

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
//typedef std::function<void (const TcpConnectionPtr&,
        //const char* data, ssize_t len)> MessageCallback;
typedef std::function<void (const TcpConnectionPtr&,
        Buffer* buf,
        Timestamp)> MessageCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;

}

#endif
