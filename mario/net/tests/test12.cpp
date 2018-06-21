#include "mario/net/EventLoop.h"
#include "mario/base/easylogging++.h"
#include "mario/net/Connector.h"

mario::EventLoop* g_loop;

void connectCallback(int sockfd) {
    LOG(INFO) << "connected";
    g_loop->quit();
}

INITIALIZE_EASYLOGGINGPP

int main() {
    mario::EventLoop loop;
    g_loop = &loop;

    mario::InetAddress addr("127.0.0.1", 9981);
    mario::ConnectorPtr connector(new mario::Connector(&loop, addr));
    connector->setNewConnectionCallback(connectCallback);
    connector->start();

    loop.loop();
}
