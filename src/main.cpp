#include <iostream>
#include <format>

#include "chatserver.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << std::format("Usage: {} ip: port.", argv[0]) << std::endl;
        return 0;
    }
    const char* ip = argv[1];
    const uint16_t port = std::stoi(argv[2]);

    muduo::net::EventLoop loop;
    const muduo::net::InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");

    // 启动服务器，接收来自外界的连接以及消息
    server.start();
    loop.loop();
}