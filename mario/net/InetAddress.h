#ifndef MARIO_NET_INETADDRESS_H
#define MARIO_NET_INETADDRESS_H

#include <string>
#include <netinet/in.h>

namespace mario {

class InetAddress {

public:
    explicit InetAddress(uint16_t port);

    InetAddress(const std::string& ip, uint16_t port);

    InetAddress(const struct sockaddr_in& addr) : _addr(addr) {}

    std::string toHostPort() const;

    const struct sockaddr_in& getSockAddrInet() const {
        return _addr;
    }

    void setSockAddrInet(const struct sockaddr_in& addr) {
        _addr = addr;
    }

private:
    struct sockaddr_in _addr;

};
}

#endif
