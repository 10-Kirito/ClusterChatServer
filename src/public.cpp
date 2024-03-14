#include "public.hpp"
#include <map>
#include <string_view>

// Not use
std::ostream &operator<<(std::ostream &out, const MessageType type) {
  static const auto strings = [] {
    std::map<MessageType, std::string_view> result;
#define INSERT_ELEMENT(p) result.emplace(p, #p);
    INSERT_ELEMENT(MessageType::LOGIN_MSG);
    INSERT_ELEMENT(MessageType::REGIST_MSG);
#undef INSERT_ELEMENT
    return result;
  };
  return out << strings()[type];
}

// Using
std::string getMsgTypeText(const MessageType type) {
  std::map<MessageType, std::string> result;
#define INSERT_ELEMENT(p) result.emplace(p, #p);
  INSERT_ELEMENT(MessageType::LOGIN_MSG);
  INSERT_ELEMENT(MessageType::REGIST_MSG);
#undef INSERT_ELEMENT
  return result[type];
}