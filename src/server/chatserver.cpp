#include "chatserver.hpp"
#include <functional>
#include <iostream>

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop) {
  // set the callback for the connection
  _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

  // set the callback for the message
  _server.setMessageCallback(
      std::bind(&ChatServer::onMessage, this, _1, _2, _3));
}

void ChatServer::start() { _server.start(); }

void ChatServer::onConnection(const TcpConnectionPtr &connection) {
    if (connection->connected()) {
      std::cout << "New connection:" << connection->peerAddress().toIpPort()
                << "->" << connection->localAddress().toIpPort()
                << " status: ONLINE" << std::endl;
    } else {
      std::cout << "New connection:" << connection->peerAddress().toIpPort()
                << "->" << connection->localAddress().toIpPort()
                << " status: OFFLINE" << std::endl;
      // close the resource
      connection->shutdown();
    }
  }
  // the callback for the message
  void ChatServer::onMessage(const TcpConnectionPtr &connection, Buffer *buffer,
                 Timestamp timestamp) {}