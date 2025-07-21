//
// Created by kirito on 25-7-22.
//
#include <format>
#include <iostream>
#include "chatserver.h"

ChatServer::ChatServer(muduo::net::EventLoop *loop, const muduo::net::InetAddress &addr, const std::string &name_arg)
    : server_(loop, addr, name_arg), loop_(loop) {
    using namespace std::placeholders;
    server_.setConnectionCallback(std::bind(&ChatServer::OnConnection, this, _1));
    server_.setMessageCallback(std::bind(&ChatServer::OnMessage, this, _1, _2, _3));
}

void ChatServer::start() {
    server_.start();
}

void ChatServer::OnConnection(const muduo::net::TcpConnectionPtr &conn) {
    if (conn->connected()) {
        std::cout << std::format("New connection from {} status: ONLINE", conn->peerAddress().toIpPort()) << std::endl;
    } else {
        std::cout << std::format("Connection disconnected {} status: OFFLINE", conn->peerAddress().toIpPort()) << std::endl;
        conn->shutdown();
    }
}

void ChatServer::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp time) {
}




