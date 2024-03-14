#include "chatserver.hpp"
#include "chatservices.hpp"
#include <functional>
#include <iostream>
#include <string>
#include <json.hpp>

using json = nlohmann::json;

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
    // close the resource when the client close the connection
    connection->shutdown();
  }
}

// the callback for the message
void ChatServer::onMessage(const TcpConnectionPtr &connection, Buffer *buffer,
                           Timestamp time) {
  std::string buf = buffer->retrieveAllAsString();
  json data = json::parse(buf);

  // get the message type and call the corresponding handler
  auto messageHandle = ChatService::getInstance().getHandler(data["msgtype"].template get<MessageType>());
  messageHandle(connection, data, time);
}