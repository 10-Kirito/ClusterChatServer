#ifndef MESSAGE_MODEL_H
#define MESSAGE_MODEL_H
#include "messge.hpp"
#include <vector>

class MessageModel {
public:
  void insert(const Message &msg);
  void deleteMsg(const Message &msg);
  void deleteMsg(const std::vector<Message>&);
  std::vector<Message> queryMsg(int id);
};

#endif // MESSAGE_MODEL_H