#include "chatservices.hpp"
#include "json.hpp"
#include "message/messge.hpp"
#include "messagemodel.hpp"
#include "public.hpp"
#include "user.hpp"
#include <muduo/base/Logging.h>
#include <mutex>
#include <vector>
using namespace std::placeholders;
using json = nlohmann::json;

// the constructor of ChatService
ChatService::ChatService() {
  // bind the message handler
  _msgHandlerMap.insert({MessageType::LOGIN_MSG,
                         std::bind(&ChatService::Login, this, _1, _2, _3)});
  _msgHandlerMap.insert({MessageType::REGIST_MSG,
                         std::bind(&ChatService::Register, this, _1, _2, _3)});
  // insert the one-chat message handler
  _msgHandlerMap.insert({MessageType::MESSAGE,
                         std::bind(&ChatService::OneChat, this, _1, _2, _3)});
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
  // LOG_INFO << "Checking user's password...";
  if (user.getId() != -1 && user.getPwd() == pwd) {
    if (user.getState() == "online") {
      // the user is already online
      LOG_INFO << "Login failed, the user is already online!";
      response["msgType"] = MessageType::LOGIN_MSG_ACK;
      response["status"] = 404;
      response["errorMessage"] = "The user already login!";
    } else {
      // login successfully and store the user's connection
      // !!!need to consider the concurrency problem
      {
        std::lock_guard<std::mutex> lock(_mutex);
        _userConnMap.insert({id, connection});
        _userMap.insert({connection, id});
      }
      // begin to send the message to the user
      LOG_INFO << "Login successful!";
      user.setState("online");
      _userModel.update(user);
      response["msgType"] = MessageType::LOGIN_MSG_ACK;
      response["status"] = 200;
      response["id"] = user.getId();
      response["name"] = user.getName();

      // get the offline message from the database
      std::vector<Message> old_message = _messageModel.queryMsg(user.getId());
      if(!old_message.empty()) {
        response["offlinemsg"] = old_message;
      }
      // delete all the offline message, because the user has already read them
      _messageModel.deleteMsg(old_message);
    }
  } else {
    LOG_INFO << "Login failed, the password is Wrong!";
    response["msgType"] = MessageType::LOGIN_MSG_ACK;
    response["status"] = 404;
    response["errorMessage"] =
        "The password is wrong or the user is not exist!";
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
  // LOG_INFO << "The data received is " << data.dump();
  std::string name = data["name"];
  std::string password = data["password"];

  User user;
  user.setName(name);
  user.setPwd(password);

  // LOG_INFO << "Begin register... ";
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
 * @brief user chat: one to one
 *
 * @param connection
 * @param data
 * @param time
 */
void ChatService::OneChat(const TcpConnectionPtr &connection, json &data,
                          Timestamp time) {
  /** the message json data structure is:
      {
        "msgtype": 4,
        "from": 1,
        "to": 2,
        "message": "hello"
      }
  */
  // handle the json data and get the user's id and the message
  Message message = data;
  int from_id = message.fromId;
  int to_id = message.toId;
  User from_user = _userModel.query(from_id);
  User to_user = _userModel.query(to_id);

  LOG_INFO << "From user" << from_user.getName() << "(" << from_id << ") to "
           << to_user.getName() << "(" << to_id << ") : " << message.message;
  // if the user is online, then send the message to the user
  if (to_user.getState() == "online") {
    {
      std::lock_guard<std::mutex> lock(_mutex);
      const TcpConnectionPtr &to_conn = _userConnMap[to_id];
      to_conn->send(data.dump());
    }
    LOG_INFO << "Send message to " << to_user.getName() << " successfully!";
  } else {
    // if the user isn't online, then store the message into database
    _messageModel.insert(message);
    LOG_INFO << "Send message to " << to_user.getName()
             << " failed because the " << to_user.getName()
             << " is not online!";
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

/**
 * @brief handle the client close exception
 * 
 * @param connection 
 */
void ChatService::clientCloseException(const TcpConnectionPtr &connection) {
  // handle the user's status:
  int id = _userMap[connection];
  User user = _userModel.query(id);
  // if the user already login:
  if (user.getId() != -1) {
    user.setState("offline");
    _userModel.update(user);
  }
  // erase the user's connection in _userConnMap:
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _userConnMap.erase(id);
    _userMap.erase(connection);
  }
}
