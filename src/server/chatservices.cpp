#include "chatservices.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
using namespace std::placeholders;

// the constructor of ChatService
ChatService::ChatService() {
  // bind the message handler
  _msgHandlerMap.insert({MessageType::LOGIN_MSG,
                         std::bind(&ChatService::Login, this, _1, _2, _3)});
  _msgHandlerMap.insert({MessageType::REGIST_MSG,
                         std::bind(&ChatService::Register, this, _1, _2, _3)});
}

/**
 * @brief the service for user login
 *
 * @param connection
 * @param data
 * @param time
 */
void ChatService::Login(const TcpConnectionPtr &connection, json &data,
                        Timestamp time) {
  LOG_INFO << "login message!";
}

/**
 * @brief the service for user register
 *
 * @param connection
 * @param data
 * @param time
 */
void ChatService::Register(const TcpConnectionPtr &connection, json &data,
                           Timestamp time) {
  LOG_INFO << "register message!";
}

/**
 * @brief get the message handler
 *
 * @param type
 * @return MessageHandler
 */
MessageHandler ChatService::getHandler(MessageType type) {
  auto it = _msgHandlerMap.find(type);
  std::string typetext = getMsgTypeText(type);
  if (it == _msgHandlerMap.end()) {
    return [=](auto a, auto b, auto c) {
      LOG_ERROR << "message type:" << typetext
                << "can not find handler!";
    };
  } else {
    return _msgHandlerMap[type];
  }
}