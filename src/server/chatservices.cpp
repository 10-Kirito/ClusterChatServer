#include "chatservices.hpp"
#include "json.hpp"
#include "public.hpp"
#include "user.hpp"
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
  LOG_INFO << "Checking Login...";
  int id = data["id"];
  std::string pwd = data["password"];

  User user = _userModel.query(id);

  json response;
  LOG_INFO << "Checking user's password...";
  if (user.getId() != -1 && user.getPwd() == pwd) {
    if (user.getState() == "online") {
      // the user is already online
      LOG_INFO << "Login failed, the user is already online!";
      response["msgType"] = MessageType::LOGIN_MSG_ACK;
      response["status"] = 404;
      response["errorMessage"] = "The user already login!";
    } else {
      LOG_INFO << "Login successful!";
      user.setState("online");
      _userModel.update(user);
      response["msgType"] = MessageType::LOGIN_MSG_ACK;
      response["status"] = 200;
      response["id"] = user.getId();
      response["name"] = user.getName();
    }
  } else {
    LOG_INFO << "Login failed, the password is Wrong!";
    response["msgType"] = MessageType::LOGIN_MSG_ACK;
    response["status"] = 404;
    response["errorMessage"] = "The password is wrong or the user is not exist!";
  }
  connection->send(response.dump());
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
  LOG_INFO << "Register start:";
  LOG_INFO << "The data received is " << data.dump();
  std::string name = data["name"];
  std::string password = data["password"];

  User user;
  user.setName(name);
  user.setPwd(password);

  LOG_INFO << "Begin register... ";
  bool state = _userModel.insert(user);
  LOG_INFO << "Finish register: the status is "
           << (state ? "success" : "failed");
  if (state) {
    // register successfully
    json response;
    response["msgType"] = MessageType::REGIST_MSG_ACK;
    response["status"] = 200;
    response["id"] = user.getId();
    connection->send(response.dump());

    LOG_INFO << "Register successfuly, the user id is " << user.getId();
  } else {
    // register failed
    json response;
    response["msgType"] = MessageType::REGIST_MSG_ACK;
    response["status"] = 404;
    connection->send(response.dump());
    LOG_INFO << "Register failed!";
  }
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
      LOG_ERROR << "message type:" << typetext << "can not find handler!";
    };
  } else {
    return _msgHandlerMap[type];
  }
}