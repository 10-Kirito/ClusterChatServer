/**
 * @file public.hpp
 * @author 10-Kirito (10.z.p.f.kirito@gmail.com)
 * @brief This file is used to define the public enum class MessageType, funtion and so on.
 * @version 0.1
 * @date 2024-03-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef PUBLIC_H
#define PUBLIC_H
#include <iostream>

/**
 * @brief The MessageType enum class
 * 
 */
enum class MessageType {
  LOGIN_MSG = 1,  // login message
  REGIST_MSG = 2, // register message
};

class Message {
  MessageType type;
  std::string content;
};

/**
 * @brief overload the opertor << for MeaaageType
 * 
 * @return std::ostream& 
 */
std::ostream &operator<<(std::ostream &, const MessageType);

/**
 * @brief Get the Msg Type Text object
 * 
 * @param type 
 * @return std::string 
 */
std::string getMsgTypeText(const MessageType type);

#endif