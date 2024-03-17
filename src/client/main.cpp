/**
 * @file main.cpp
 * @author 10-Kirito (10.z.p.f.kirito@gmail.com)
 * @brief chat client main file
 * @version 0.1
 * @date 2024-03-17
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "groups/group.hpp"
#include "messge.hpp"
#include "public.hpp"
#include "user.hpp"
#include <arpa/inet.h>
#include <atomic>
#include <cstring>
#include <functional>
#include <iostream>
#include <json.hpp>
#include <netinet/in.h>
#include <semaphore.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using json = nlohmann::json;

// control the main menu running status
bool isMainMenuRunning = false;

// the global user and friends list
User global_user;
std::vector<User> global_userfriends_list;
std::vector<Group> global_group_list;

// the global login status
std::atomic_bool global_login_status{false};
// the read-write lock
sem_t rwsem;

// ---- the function maybe use:
// --the read task handler:
void readTaskHandler(int clientfd);
// --the home page:
void homePage(int);
// --the system time:
std::string getCurrentTime();

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " ip: port" << std::endl;
  }
  // input the IP and PORT:
  char *ip = argv[1];
  uint16_t port = std::stoi(argv[2]);
  int clientfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == clientfd) {
    std::cerr << "socket create error" << std::endl;
    exit(-1);
  }

  sockaddr_in server;
  memset(&server, 0, sizeof(sockaddr_in));

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = inet_addr(ip);
  // connect to the server
  if (-1 == connect(clientfd, (sockaddr *)&server, sizeof(sockaddr_in))) {
    std::cerr << "connect server error" << std::endl;
    close(clientfd);
    exit(-1);
  }
  // create the read-write lock
  sem_init(&rwsem, 0, 0);
  // connect the server successfully, then start the thread
  std::thread readTask(readTaskHandler, clientfd);
  readTask.detach();
  // main thread used to send message, and the readTask thread used to receive
  // data from server
  for (;;) {
    // display the main menu: 1. login; 2. register; 3. quit
    cout << "----------------------------" << endl;
    cout << "1. login" << endl;
    cout << "2. register" << endl;
    cout << "3. quit" << endl;
    cout << "----------------------------" << endl;
    cout << "choice:";
    int choice = 0;
    cin >> choice;
    cin.get(); // remove the '\n' in the input buffer
    switch (choice) {
      // login
    case 1: {
      int id = 0;
      char pwd[50] = {0};
      cout << "account:";
      cin >> id;
      cin.get(); // remove the '\n' in the input buffer
      cout << "password:";
      cin.getline(pwd, 50);

      json js;
      js["msgtype"] = MessageType::LOGIN_MSG;
      js["id"] = id;
      js["password"] = pwd;
      std::string request = js.dump();

      global_login_status = false;

      int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
      if (len == -1) {
        std::cerr << "send login msg error:" << request << endl;
      }
      // wait for the readTask thread to process the login response
      sem_wait(&rwsem);

      if (global_login_status) {
        // go home page
        isMainMenuRunning = true;
        homePage(clientfd);
      }
    } break;
    case 2: // register业务
    {
      char name[50] = {0};
      char pwd[50] = {0};
      cout << "username:";
      cin.getline(name, 50);
      cout << "userpassword:";
      cin.getline(pwd, 50);

      json js;
      js["msgid"] = MessageType::REGIST_MSG;
      js["name"] = name;
      js["password"] = pwd;
      std::string request = js.dump();

      int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
      if (len == -1) {
        std::cerr << "send reg msg error:" << request << endl;
      }

      sem_wait(&rwsem); // 等待信号量，子线程处理完注册消息会通知
    } break;
    case 3: // quit业务
      close(clientfd);
      sem_destroy(&rwsem);
      exit(0);
    default:
      std::cerr << "invalid input!" << endl;
      break;
    }
  }

  return 0;
}

// display the current user's information
void showCurrentUserData() {
  cout << "----------------------login user---------------------" << endl;
  cout << "current login user => id:" << global_user.getId()
       << " name:" << global_user.getName() << endl;
  cout << "----------------------friend list---------------------" << endl;
  if (!global_userfriends_list.empty()) {
    for (User &user : global_userfriends_list) {
      cout << user.getId() << " " << user.getName() << " " << user.getState()
           << endl;
    }
  }
  cout << "----------------------group list----------------------" << endl;
  if (!global_group_list.empty()) {
    for (Group &group : global_group_list) {
      cout << group.getId() << " " << group.getName() << " " << group.getDesc()
           << endl;
    }
  }
  cout << "------------------------------------------------------" << endl;
}

// handle with the login response
void doLoginResponse(json &responsejs) {
  // login failed
  if (200 != responsejs["status"].get<int>()) {
    std::cerr << responsejs["status"] << endl;
    global_login_status = false;
    // login successfully
  } else {
    // 0.record the current user's information
    global_user.setId(responsejs["id"].template get<int>());
    global_user.setName(responsejs["name"].template get<std::string>());

    // 1.record the current user's friends list
    global_userfriends_list.clear();
    if (responsejs.contains("friends")) {
      global_userfriends_list =
          responsejs["friends"].template get<std::vector<User>>();
    }

    // record the all groups
    if (responsejs.contains("groups")) {
      // clear
      global_group_list.clear();
      global_group_list =
          responsejs["groups"].template get<std::vector<Group>>();
    }

    // display the user's information

    // display offline message:
    if (responsejs.contains("offlinemsg")) {
      std::vector<Message> vec = responsejs["offlinemsg"];
      for (Message &message : vec) {
        // time + [id] + name + " said: " + xxx
        if (MessageType::MESSAGE == MessageType(message.msgtype)) {
          cout << message.time << " [" << message.fromId << "]"
               << " said: " << message.message << endl;
        } else {
          cout << "group messages[" << message.fromId << "]:" << message.time
               << " said: " << message.message << endl;
        }
      }
    }

    global_login_status = true;
  }
}

// the thread receive the data from server
void readTaskHandler(int clientfd) {
  for (;;) {
    char buffer[1024] = {0};
    // block here to reveive the data from server
    int len = recv(clientfd, buffer, 1024, 0);
    if (-1 == len || 0 == len) {
      close(clientfd);
      exit(-1);
    }
    // receive the data from server and then process the data
    json js = json::parse(buffer);
    MessageType msgtype = js["msgtype"].get<MessageType>();

    if (MessageType::LOGIN_MSG_ACK == msgtype) {
      // login response:
      doLoginResponse(js);
      // notify the main thread:
      sem_post(&rwsem);
      continue;
    }

    if (MessageType::MESSAGE == msgtype) {
      cout << js["time"].get<std::string>() << " [" << js["from"] << "]"
           << js["name"].get<std::string>()
           << " said: " << js["message"].get<std::string>() << endl;
      continue;
    }

    /* if (GROUP_CHAT_MSG == msgtype) {
      cout << "群消息[" << js["groupid"] << "]:" << js["time"].get<string>()
           << " [" << js["id"] << "]" << js["name"].get<string>()
           << " said: " << js["msg"].get<string>() << endl;
      continue;
    }



    if (REG_MSG_ACK == msgtype) {
      doRegResponse(js);
      sem_post(&rwsem); // 通知主线程，注册结果处理完成
      continue;
    } */
  }
}

// "help" command handler
void help(int fd = 0, std::string str = "");
// "chat" command handler
void chat(int, std::string);

// the command lists
std::unordered_map<std::string, std::string> commandMap = {
    {"help", "get the help list. Usage: help"},
    {"chat", "chat with someone. Usage: chat:friendid::message"},
    {"groupchat", "chat with group. Usage: groupchat:groupid::message"},
   /*  {"addfriend", "add friend"},
    {"creategroup", "create group"},       {"addgroup", "add group"},
    {"groupmembers", "get group members"}, {"loginout", "login out"},
    {"quit", "quit the chat system"} */};

std::unordered_map<std::string, std::function<void(int, std::string)>>
    commandHandlerMap = {{"help", help}, {"chat", chat}};

void homePage(int clientfd) {}

// "help" command handler
void help(int, std::string) {
  cout << "show command list >>> " << endl;
  for (auto &p : commandMap) {
    cout << p.first << " : " << p.second << endl;
  }
  cout << endl;
}

/**
 * @brief one to one chat
 *
 * @param clientfd
 * @param str (friendid:message)
 */
// "chat" command handler
void chat(int clientfd, std::string str) {
  int idx = str.find(":"); // friendid:message
  if (-1 == idx) {
    std::cerr << "chat command invalid!" << endl;
    return;
  }

  // handle the command the user input
  int friendid = atoi(str.substr(0, idx).c_str());
  std::string message = str.substr(idx + 1, str.size() - idx);

  json js;
  js["msgtype"] = MessageType::MESSAGE;
  js["from"] = global_user.getId();
  js["to"] = friendid;
  js["message"] = message;
  // new add
  js["time"] = getCurrentTime();
  string buffer = js.dump();
  int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
  if (-1 == len) {
    std::cerr << "send chat msg error -> " << buffer << endl;
  }
}

/**
 * @brief Get the Current Time object
 *
 * @return std::string
 */
// system time
std::string getCurrentTime() {
  auto tt =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  struct tm *ptm = localtime(&tt);
  char date[60] = {0};
  sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", (int)ptm->tm_year + 1900,
          (int)ptm->tm_mon + 1, (int)ptm->tm_mday, (int)ptm->tm_hour,
          (int)ptm->tm_min, (int)ptm->tm_sec);
  return std::string(date);
}