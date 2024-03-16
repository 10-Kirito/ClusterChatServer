/**
 * @file messge.hpp
 * @author 10-Kirito (10.z.p.f.kirito@gmail.com)
 * @brief This file defines the structure of the message between the client and
 * the server.
 * @version 0.1
 * @date 2024-03-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef MESSAGE_H
#define MESSAGE_H
#include <string>
#include <json.hpp>

using json = nlohmann::json;

struct Message {
  int fromId;
  int toId;
  std::string message;
  int msgtype;
};

inline void to_json(json &j, const Message &p) {
  j = json{{"msgtype", p.msgtype},
           {"from", p.fromId},
           {"to", p.toId},
           {"message", p.message}};
}

inline void from_json(const json &j, Message &p) {
  j.at("msgtype").get_to(p.msgtype);
  j.at("from").get_to(p.fromId);
  j.at("to").get_to(p.toId);
  j.at("message").get_to(p.message);
}
#endif // MESSAGE_H