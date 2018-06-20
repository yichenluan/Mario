#include "mario/net/EventLoop.h"
#include "mario/net/InetAddress.h"
#include "mario/net/TcpServer.h"
#include "mario/base/CurrentThread.h"
#include "mario/base/easylogging++.h"

#include <unistd.h>

void onConnection(const mario::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOG(INFO) << "onConnection(): tid = " << mario::CurrentThread::tid() 
            <<  "new connection " << conn->name() << ", from " << conn->peerAddress().toHostPort();
    } else {
        LOG(INFO) << "onConnection(): tid = " << mario::CurrentThread::tid() 
            << ", connection " << conn->name() << " is down";
    }
}

void onMessage(const mario::TcpConnectionPtr& conn,
        mario::Buffer* buf,
        mario::Timestamp receiveTime) {
    LOG(INFO) << "onMessage(): tid = " << mario::CurrentThread::tid() 
        << ", received " << buf->readableBytes() << " bytes from connection " << conn->name() << " at " << receiveTime.toFormattedString();
    conn->send(buf->retrieveAsString());
}

INITIALIZE_EASYLOGGINGPP

int main() {
    LOG(INFO) << "main: pid = "<< getpid() << ", tid = " << mario::CurrentThread::tid();

    mario::InetAddress listenAddr(9981);
    mario::EventLoop loop;

    mario::TcpServer server(&loop, listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.setThreadNum(3);
    server.start();

    loop.loop();
}
