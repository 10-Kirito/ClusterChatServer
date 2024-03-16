#include "public.hpp"
#include <map>
#include <string_view>
#include <cppconn/connection.h>
#include <muduo/base/Logging.h>

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

/**
 * @brief printf the error message
 *
 * @param e
 */
void logError(const sql::SQLException &e) {
  LOG_INFO << "\t\t ***Error: SQLException in " << __FILE__;
  LOG_INFO << "\t\t ***The " << __FUNCTION__ << "() on line " << __LINE__;
  LOG_INFO << "\t\t ***Error: " << e.what();
  LOG_INFO << "\t\t ***(MySQL error codes: " << e.getErrorCode() << ", SQLState: " << e.getSQLState() << ")";
}