#ifndef CHATSERVICES_H
#define CHATSERVICES_H

#include <functional> // std::function
#include <json.hpp> // nlohmann::json
#include <muduo/base/Timestamp.h> // Timestamp
#include <muduo/net/Callbacks.h> // TcpConnectionPtr
#include <muduo/net/TcpConnection.h> // TcpConnectionPtr
#include <mutex> // mutex
#include <unordered_map> // unordered_map

#include "messagemodel.hpp"
#include "singletontemplate.hpp" // SingletonTemplate
#include "usermodel.hpp" // UserModel
#include "public.hpp" // MessageType
// Remove the problematic include statement
// #include "messagemodel.hpp"

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

  // service for user chat
  void OneChat(const TcpConnectionPtr &, json &, Timestamp);

  // get the message handler
  MessageHandler getHandler(MessageType type);
  // when the client close the connection, delete something
  void clientCloseException(const TcpConnectionPtr &connection);
private:
  ChatService();
  // the message handler map
  std::unordered_map<MessageType, MessageHandler> _msgHandlerMap;
  // store online user's connection
  std::unordered_map<int, TcpConnectionPtr> _userConnMap;
  std::unordered_map<TcpConnectionPtr, int> _userMap;

  UserModel _userModel;

  MessageModel _messageModel;

  std::mutex _mutex;
};

#endif // CHATSERVICES_H