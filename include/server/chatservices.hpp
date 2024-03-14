#ifndef CHATSERVICES_H
#define CHATSERVICES_H

#include <functional>
#include <json.hpp>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpConnection.h>
#include <unordered_map>

#include "SingletonTemplate.hpp"
#include "public.hpp"

using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

// the message handler
using MessageHandler =
    std::function<void(const TcpConnectionPtr &, json &, Timestamp)>;

// the services of server
class ChatService : public SingletonTemplate<ChatService> {
  // friend class of SingletonTemplate, and SingletonTemplate<ChatService> can
  // access the private members of ChatService
  friend class SingletonTemplate<ChatService>;

public:
  // service for user login
  void Login(const TcpConnectionPtr &, json &, Timestamp);

  // service for user register
  void Register(const TcpConnectionPtr &, json &, Timestamp);


  // get the message handler
  MessageHandler getHandler(MessageType type);

private:
  ChatService();
  // the message handler map
  std::unordered_map<MessageType, MessageHandler> _msgHandlerMap;
};

#endif // CHATSERVICES_H