#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

using namespace std::placeholders;

class ChatServer {
public:
  // the contructor initializes the TcpServer with the EventLoop and the
  // InetAddress
  /*
        @param loop: the EventLoop that the server will use to listen for new
     connections
        @param listenAddr: the InetAddress that the server will listen on
        @param nameArg: the name of the server
  */
  ChatServer(EventLoop *loop, const InetAddress &listenAddr,
             const string &nameArg);
  void start();

private:
  // the callback for the connection
  void onConnection(const TcpConnectionPtr &connection);
  // the callback for the message
  void onMessage(const TcpConnectionPtr &connection, Buffer *buffer,
                 Timestamp timestamp);

  TcpServer _server;
  EventLoop *_loop;
};

#endif // CHATSERVER_H