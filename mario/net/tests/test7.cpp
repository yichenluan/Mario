#include "mario/net/EventLoop.h"
#include "mario/net/Acceptor.h"
#include "mario/net/InetAddress.h"
#include "mario/net/SocketsOps.h"
#include "mario/base/CurrentThread.h"
#include "mario/base/easylogging++.h"

#include <unistd.h>

void newConnection(int sockfd, const mario::InetAddress& peerAddr) {
    LOG(INFO) << "newConnection(): accepted a new connection from " << peerAddr.toHostPort();
    ::write(sockfd, "How are you?\n", 13);
    mario::sockets::close(sockfd);
}

INITIALIZE_EASYLOGGINGPP

int main() {
    LOG(INFO) << "main: pid = "<< getpid() << ", tid = " << mario::CurrentThread::tid();

    mario::InetAddress listenAddr(9981);
    mario::EventLoop loop;

    mario::Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();

    loop.loop();
}
