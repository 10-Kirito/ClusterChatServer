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
#include <cstdlib>
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
void quit(int clientfd, std::string str = "");

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
  // create the read-write lock, and initialize the value to 0
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
    case 2: // register
    {
      char name[50] = {0};
      char pwd[50] = {0};
      cout << "username:";
      cin.getline(name, 50);
      cout << "userpassword:";
      cin.getline(pwd, 50);

      json js;
      js["msgtype"] = MessageType::REGIST_MSG;
      js["name"] = name;
      js["password"] = pwd;
      std::string request = js.dump();

      int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
      if (len == -1) {
        std::cerr << "send reg msg error:" << request << endl;
      }

      sem_wait(&rwsem); // 等待信号量，子线程处理完注册消息会通知
    } break;
    case 3: // quit
      quit(clientfd);
    default:
      std::cerr << "invalid input!" << endl;
      break;
    }
  }
  return 0;
}

void quit(int clientfd, std::string str) {
  close(clientfd);
  sem_destroy(&rwsem);
  exit(0);
}

// display the current user's information
void showCurrentUserData(int, std::string) {
  cout << endl;
  cout << "----------------------login user---------------------" << endl;
  cout << "current user => account:" << global_user.getId()
       << " username:" << global_user.getName() << endl;
  cout << "----------------------friend list---------------------" << endl;

  if (!global_userfriends_list.empty()) {
    cout << "id\t\tname\t\tstate" << endl;
    for (User &user : global_userfriends_list) {
      cout << user.getId() << "\t\t" << user.getName() << "\t\t"
           << user.getState() << endl;
    }
  }
  cout << "----------------------group list----------------------" << endl;
  if (!global_group_list.empty()) {
    cout << "id\t\tname\t\t\tdesc" << endl;
    for (Group &group : global_group_list) {
      cout << group.getId() << "\t\t" << group.getName() << "\t\t\t"
           << group.getDesc() << endl;
    }
  }
  cout << "------------------------------------------------------" << endl;
  cout << "Type `help` to see other commands." << endl;
}

// handler the register response
void doRegResponse(json &responsejs) {
  if (200 != responsejs["status"].get<int>()) // register failed
  {
    std::cerr << "register error!" << endl;
  } else // register successfully
  {
    cout << "name register success, userid is " << responsejs["id"]
         << ", do not forget it!" << endl;
  }
}

// handle with the login response
void doLoginResponse(json &responsejs) {
  // login failed
  if (400 == responsejs["status"].get<int>()) {
    std::cerr << "(" << responsejs["status"]
              << ")The password is wrong or the user is not exist!" << endl;
    global_login_status = false;
    // login successfully
  } else if (401 == responsejs["status"].get<int>()) {
    std::cerr << "(" << responsejs["status"] << ")The user is already online!"
              << endl;
    global_login_status = false;
  } else {
    // 0.record the current user's information
    global_user.setId(responsejs["id"].template get<int>());
    global_user.setName(responsejs["name"].template get<std::string>());

    // 1.record the current user's friends list
    global_userfriends_list.clear();
    if (responsejs.contains("friends")) {
      auto test = responsejs["friends"];
      std::cout << responsejs["friend"];
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
    showCurrentUserData(1, "");

    // display offline message:
    if (responsejs.contains("offlinemsg")) {
      std::vector<Message> vec = responsejs["offlinemsg"];
      for (Message &message : vec) {
        // time + [id] + name + " said: " + xxx
        cout << message.time << " [" << message.fromId << "]:"
             << " said: " << message.message << endl;
      }
    }

    // display offline group message:
    if (responsejs.contains("groupmsg")) {
      /*
      {
        [
          {

          }
        ],
        [
          {

          }
        ]
      }
      */
      std::vector<std::vector<Message>> groupmsg = responsejs["groupmsg"];

      cout << "------------------------------------------------------" << endl;
      for (const auto &onegroup : groupmsg) {
        cout << "group message[" << onegroup[0].toId << "]:" << endl;
        for (const auto &message : onegroup) {
          cout << "├──" << message.time << " [" << message.fromId << "]:"
               << " said: " << message.message << endl;
        }
      }
      cout << "------------------------------------------------------" << endl;
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

    if (MessageType::REGIST_MSG_ACK == msgtype) {
      doRegResponse(js);
      sem_post(&rwsem);
      continue;
    }

    if (MessageType::MESSAGE == msgtype) {
      Message message = js;
      cout << js["time"].get<std::string>() << " [" << js["from"] << "]"
           << " said: " << js["message"].get<std::string>() << endl;
      continue;
    }

    // update the user's information
    if (MessageType::UPDATE_USER_ACK == msgtype) {
      global_userfriends_list.clear();
      global_group_list.clear();
      global_userfriends_list = js["friends"].get<std::vector<User>>();
      global_group_list = js["groups"].get<std::vector<Group>>();
      continue;
    }
    if (MessageType::GROUP_MESSAGE == msgtype) {
      cout << "group message[" << js["to"] << "]:" << js["time"].get<string>()
           << " from [" << js["from"] << "]"
           << " said: " << js["message"].get<string>() << endl;
      continue;
    }
  }
}

// "help" command handler
void help(int fd = 0, std::string str = "");
// "chat" command handler
void chat(int, std::string);
// "groupchat" command handler
void groupChat(int, std::string);
// "list" command handler
void showUsersInGroup(int, std::string);
// "update" command handler
void update(int, std::string);
// the command lists
std::unordered_map<std::string, std::string> commandMap = {
    {"quit", "\t(exit the chat)"},
    {"chat", "\t(chat with someone) usage-> `chat:friendid:message`"},
    {"groupchat", "(chat with group) usage> `groupchat:groupid:message`"},
    {"showme", "(show my details) usage> `showme`"},
    {"help", "\t(get the help list) usage-> `help`"},
    {"list", "(get the users in the group) usge-> `list:groupid`"},
    {"updatelist", "(update the user's list) usage-> `updatelist`"}
   /*  {"addfriend", "add friend"},
    {"creategroup", "create group"},       {"addgroup", "add group"},
    {"groupmembers", "get group members"}, {"loginout", "login out"},
    {"quit", "quit the chat system"} */};

std::unordered_map<std::string, std::function<void(int, std::string)>>
    commandHandlerMap = {{"help", help},
                         {"chat", chat},
                         {"quit", quit},
                         {"showme", showCurrentUserData},
                         {"list", showUsersInGroup},
                         {"groupchat", groupChat},
                         {"update", update}};

void homePage(int clientfd) {
  help();
  char buffer[1024] = {0};
  while (isMainMenuRunning) {
    cin.getline(buffer, 1024);
    string commandbuf(buffer);
    // store the commands
    string command;
    int idx = commandbuf.find(":");
    if (-1 == idx) {
      command = commandbuf;
    } else {
      command = commandbuf.substr(0, idx);
    }
    auto it = commandHandlerMap.find(command);
    if (it == commandHandlerMap.end()) {
      std::cerr << "invalid input command!" << endl;
      continue;
    }
    // handle the command the user input, and input the user's input
    it->second(clientfd, commandbuf.substr(idx + 1, commandbuf.size() - idx));
  }
}

// "help" command handler
void help(int, std::string) {
  cout << endl;
  cout << "                    <<<<show command list >>> " << endl;
  for (auto &p : commandMap) {
    cout << p.first << " : \t" << p.second << endl;
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

void groupChat(int clientfd, std::string str) {
  // groupid:message
  int idx = str.find(":");
  if (-1 == idx) {
    std::cerr << "groupchat command invalid" << endl;
    return;
  }
  // get the groupid and message
  int groupid = atoi(str.substr(0, idx).c_str());
  std::string message = str.substr(idx + 1, str.size() - idx);

  json js;
  js["msgtype"] = MessageType::GROUP_MESSAGE;
  js["from"] = global_user.getId();
  js["to"] = groupid;
  js["message"] = message;
  js["time"] = getCurrentTime();
  std::string buffer = js.dump();
  int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
  if (-1 == len) {
    std::cerr << "send chat msg error -> " << buffer << endl;
  }
}

void showUsersInGroup(int clientfd, std::string str) {
  // str: groupid
  cout << "------------------------------------------------------" << endl;
  int groupid = atoi(str.c_str());
  for (const auto &group : global_group_list) {
    if (group.getId() == groupid) {
      cout << "group: " << group.getName()
           << " has the following members: " << endl;
      cout << "id\t\tname\t\tstate" << endl;
      for (const auto &user : group.getUsers()) {
        cout << user.getId() << "\t\t" << user.getName() << "\t\t"
             << user.getState() << endl;
      }
      cout << "------------------------------------------------------" << endl;
      cout << "Type `help` to see other commands." << endl;
      return;
    }
  }
  cout << "group not found!" << endl;
}

/**
 * @brief update the user's information
 *
 * @param clientfd
 */
void update(int clientfd, std::string) {
  json data;
  data["msgtype"] = MessageType::UPDATE_USER;
  data["id"] = global_user.getId();
  std::string buffer = data.dump();
  int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
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