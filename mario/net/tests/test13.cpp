#include "mario/net/EventLoop.h"
#include "mario/base/easylogging++.h"
#include "mario/net/TcpClient.h"
#include "mario/base/easylogging++.h"

#include <functional>
#include <unistd.h>

std::string message = "Hello\n";

void onConnection(const mario::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOG(INFO) << "onConnection(): tid = " << mario::CurrentThread::tid() 
            <<  "new connection " << conn->name() << ", from " << conn->peerAddress().toHostPort();
        conn->send(message);
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
    LOG(INFO) << "onMessage:: [" << buf->retrieveAsString() << "]";
}

INITIALIZE_EASYLOGGINGPP

int main() {
    mario::EventLoop loop;
    mario::InetAddress serverAddr("127.0.0.1", 9981);
    mario::TcpClient client(&loop, serverAddr);

    client.setConnectionCallback(onConnection);
    client.setMessageCallback(onMessage);
    client.enableRetry();
    client.connect();
    loop.loop();
}
