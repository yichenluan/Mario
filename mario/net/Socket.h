#ifndef MARIO_NET_SOCKET_H
#define MARIO_NET_SOCKET_H

namespace mario {

class InetAddress;

class Socket {

public:
    explicit Socket(int sockfd)
        : _sockfd(sockfd)
    {}

    ~Socket();

    int fd() const {
        return _sockfd;
    }

    void bindAddress(const InetAddress& localaddr);

    void listen();

    int accept(InetAddress* peeraddr);

    void setReuseAddr(bool on);

    void shutdownWrite();

private:
    const int _sockfd;
};

}

#endif
