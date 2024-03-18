#include "chatservices.hpp"
#include "groups/group.hpp"
#include "groups/groupuser.hpp"
#include "json.hpp"
#include "message/messge.hpp"
#include "messagemodel.hpp"
#include "public.hpp"
#include "user.hpp"
#include <iomanip>
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
  _msgHandlerMap.insert({MessageType::GROUP_MESSAGE,
                         std::bind(&ChatService::GroupChat, this, _1, _2, _3)});
  // insert the AddFriend handler
  _msgHandlerMap.insert({MessageType::ADD_FRIENDS,
                         std::bind(&ChatService::AddFriend, this, _1, _2, _3)});
  // insert the QueryFriends handler
  _msgHandlerMap.insert(
      {MessageType::QUERY_FRIENDS,
       std::bind(&ChatService::QueryFriends, this, _1, _2, _3)});

  // insert the CreateGroup handler
  _msgHandlerMap.insert(
      {MessageType::CREATE_GROUP,
       std::bind(&ChatService::CreateGroup, this, _1, _2, _3)});
  _msgHandlerMap.insert({MessageType::JOIN_GROUP,
                         std::bind(&ChatService::JoinGroup, this, _1, _2, _3)});
  _msgHandlerMap.insert(
      {MessageType::DELETE_GROUP,
       std::bind(&ChatService::DeleteGroup, this, _1, _2, _3)});
  _msgHandlerMap.insert({MessageType::QUIT_GROUP,
                         std::bind(&ChatService::QuitGroup, this, _1, _2, _3)});

  _msgHandlerMap.insert(
      {MessageType::UPDATE_USER,
       std::bind(&ChatService::UpdateUser, this, _1, _2, _3)});
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
      response["msgtype"] = MessageType::LOGIN_MSG_ACK;
      response["status"] = 401;
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
      response["msgtype"] = MessageType::LOGIN_MSG_ACK;
      response["status"] = 200;
      response["id"] = user.getId();
      response["name"] = user.getName();

      // 1. get the offline message from the database
      std::vector<Message> old_message = _messageModel.queryMsg(user.getId());
      if (!old_message.empty()) {
        response["offlinemsg"] = old_message;
      }
      // delete all the offline message, because the user has already read them
      _messageModel.deleteMsg(old_message);
      // 2. get the user's all friends
      std::vector<User> friends = _friendModel.query(user.getId());
      if (!friends.empty()) {
        response["friends"] = friends;
      }
      // 3. get the user's all groups
      std::vector<Group> groups = _groupModel.queryGroups(user.getId());
      if (!groups.empty()) {
        response["groups"] = groups;
        // get the user's all groups' messages
        for (const auto &group : groups) {
          std::vector<Message> old_group_message =
              _messageModel.queryGroupMsg(group.getId(), user.getId());
          if (!old_group_message.empty()) {
            response["groupmsg"].push_back(old_group_message);
            _messageModel.deleteGroupMsg(old_group_message);
          }
        }
      }
    }
  } else {
    LOG_INFO << "Login failed, the password is Wrong!";
    response["msgtype"] = MessageType::LOGIN_MSG_ACK;
    response["status"] = 400;
    response["errorMessage"] =
        "The password is wrong or the user is not exist!";
  }
  // std::cout << std::setw(4) << response;
  connection->send(response.dump());
}

/**
 * @brief update the user's information(friend's state)
 *
 * @param connection
 * @param data
 * @param time
 */
void ChatService::UpdateUser(const TcpConnectionPtr &connection, json &data,
                             Timestamp time) {
  int id = data["id"];
  User user = _userModel.query(id);
  json response;
  // 1. get the user
  response["msgtype"] = MessageType::UPDATE_USER_ACK;
  response["user"] = user;

  // 2. get the user's all friends
  std::vector<User> friends = _friendModel.query(user.getId());
  if (!friends.empty()) {
    response["friends"] = friends;
  }
  // 3. get the user's all groups
  std::vector<Group> groups = _groupModel.queryGroups(user.getId());
  if (!groups.empty()) {
    response["groups"] = groups;
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
    response["msgtype"] = MessageType::REGIST_MSG_ACK;
    response["status"] = 200;
    response["id"] = user.getId();
    connection->send(response.dump());

    LOG_INFO << "Register successfuly, the user id is " << user.getId();
  } else {
    // register failed
    json response;
    response["msgtype"] = MessageType::REGIST_MSG_ACK;
    response["status"] = 500;
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
  std::string t = message.time;

  LOG_INFO << t << "From user" << from_user.getName() << "(" << from_id
           << ") to " << to_user.getName() << "(" << to_id
           << ") : " << message.message;
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
 * @brief the service for group chat
 *
 * @param connection
 * @param data
 * @param time
 */
void ChatService::GroupChat(const TcpConnectionPtr &connection, json &data,
                            Timestamp time) {
  /*
    {
      "msgtype": GROUP_MESSAGE
      "from": id,
      "to": groupid,
      "message": "hello, group!"
      "time": time
    }
  */
  // 1. get the all need data from json
  int userid = data["from"].template get<int>();
  int groupid = data["to"].template get<int>();
  // 2. get the users in the group
  std::vector<User> users = _groupModel.query(groupid).getUsers();
  std::vector<int> user_id;
  for (auto &user : users) {
    user_id.push_back(user.getId());
  }

  // 3. send the message to the users in the group
  std::lock_guard<std::mutex> lock(_mutex);
  for (auto &id : user_id) {
    if (id != userid) {
      auto it = _userConnMap.find(id);
      if (it != _userConnMap.end()) {
        _userConnMap[id]->send(data.dump());
      } else {
        // store the message into the database
        Message message;
        message.fromId = userid;
        message.toId = groupid;
        message.msgtype = MessageType::GROUP_MESSAGE;
        message.message = data["message"].template get<std::string>();
        message.time = data["time"].template get<std::string>();
        _messageModel.insert(message);
      }
    }
  }
}

/**
 * @brief the funtion to used to add friends
 *
 * @param connection
 * @param data
 * @param time
 */
void ChatService::AddFriend(const TcpConnectionPtr &connection, json &data,
                            Timestamp time) {
  // acquire the user's id and the friend's id
  int from_id = data["from"];
  int to_id = data["to"];

  _friendModel.insert({from_id, to_id});

  // send the message to the user
}

/**
 * @brief query friends
 *
 * @param connection
 * @param data
 * @param time
 */
void ChatService::QueryFriends(const TcpConnectionPtr &connection, json &data,
                               Timestamp time) {
  int user_id = data["id"];
  std::vector<User> result = _friendModel.query(user_id);

  json response;

  response["friends"] = result;
  connection->send(response.dump());
}

/**
 * @brief create a group
 *
 * @param connection
 * @param data
 * @param time
 */
void ChatService::CreateGroup(const TcpConnectionPtr &connection, json &data,
                              Timestamp time) {
  /*
  {
    "msgtype": 8,
    "userid": 1,
    "groupname": "group1",
    "groupdesc": "group1 desc",
  }
  */

  json response;
  // 0. get the all need data from json
  int user_id = data["userid"].template get<int>();
  std::string group_name = data["groupname"].template get<std::string>();
  std::string group_desc = data["groupdesc"].template get<std::string>();

  // 1. check if the user exist?
  bool userexist = _userModel.exist(user_id);
  response["userexist"] = userexist;
  // if the user don't exists, return
  if (!userexist) {
    response["status"] = 404;
    connection->send(response.dump());
    return;
  }
  // 2. create the group and update the AllGroup and GroupUser tables
  Group group{group_name, group_desc};
  bool result = _groupModel.create(group);
  if (result) {
    // if the group create successfully, then add the user to the group
    GroupUser groupuser{group.getId(), user_id, "creator"};
    _groupUserModel.insert(groupuser);
    response["Group"] = group;
  }
  response["status"] = result ? 200 : 404;
}

/**
 * @brief join the group
 *
 * @param connection
 * @param data
 * @param time
 */
void ChatService::JoinGroup(const TcpConnectionPtr &connection, json &data,
                            Timestamp time) {
  /*
  {
    "msgtype": 9,
    "groupid": groupid,
    "userid": userid
  }
  */
  json response;
  // 0. get the all need data from json
  int groupid = data["groupid"].template get<int>();
  int userid = data["userid"].template get<int>();
  // 1. check the user or group if exist:
  if (!_userModel.exist(userid)) {
    response["userexist"] = false;
    connection->send(response.dump());
    return;
  }
  if (!_groupModel.exist(groupid)) {
    response["groupdexist"] = false;
    connection->send(response.dump());
    return;
  }
  // 2. the user and the group both exist, then add the user to the group
  if (_groupUserModel.insert({groupid, userid, "normal"})) {
    response["status"] = 200;
    response["message"] = "already join the group!";
  }
  connection->send(response.dump());
}

void ChatService::DeleteGroup(const TcpConnectionPtr &connection, json &data,
                              Timestamp time) {}
void ChatService::QuitGroup(const TcpConnectionPtr &connection, json &data,
                            Timestamp time) {}

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
