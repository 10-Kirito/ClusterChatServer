//
// Created by kirito on 25-7-21.
//

#ifndef CHATSERVER_H
#define CHATSERVER_H

#include<string>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

class ChatServer : public std::enable_shared_from_this<ChatServer> {
public:
    ChatServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress& addr, const std::string& name_arg);
    ~ChatServer() = default;

    void start();
private:
    /**
     * TcpServer建立新链接时候对应的回调函数.
     * @param conn 建立的Tcp连接
     */
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);

    /**
     *  TcpServer收到消息时候对应的回调函数.
     * @param conn 建立的Tcp连接
     * @param buffer 接收消息对应的缓冲区
     * @param time 时间戳
     */
    void OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer* buffer, muduo::Timestamp time);

    muduo::net::TcpServer server_;
    muduo::net::EventLoop *loop_;
};

#endif //CHATSERVER_H
