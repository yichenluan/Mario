#include "mario/net/Socket.h"

#include "mario/net/InetAddress.h"
#include "mario/net/SocketsOps.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>

using namespace mario;

Socket::~Socket() {
    sockets::close(_sockfd);
}

void Socket::bindAddress(const InetAddress& addr) {
    sockets::bindOrDie(_sockfd, addr.getSockAddrInet());
}

void Socket::listen() {
    sockets::listenOrDie(_sockfd);
}

int Socket::accept(InetAddress* peeraddr) {
    // 值-结果参数
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    int connfd = sockets::accept(_sockfd, &addr);

    peeraddr->setSockAddrInet(addr);
    return connfd;
}

void Socket::setReuseAddr(bool on) {
    int optVal = on ? 1 : 0;
    ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
}
