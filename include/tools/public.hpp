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
#include <cppconn/connection.h>

/**
 * @brief The MessageType enum class
 * 
 */
enum class MessageType {
  // about register and login
  LOGIN_MSG = 0,  // login message
  LOGIN_MSG_ACK = 1,  // login message
  REGIST_MSG = 2, // register message
  REGIST_MSG_ACK = 3, // register message ack

  // about message:
  MESSAGE = 4, // message
  GROUP_MESSAGE = 5, // message

  // about friends:
  ADD_FRIENDS = 6, // add friends
  QUERY_FRIENDS = 7, // query friends

  // about group:
  CREATE_GROUP = 8,
  JOIN_GROUP = 9,
  DELETE_GROUP = 10,
  QUIT_GROUP = 11,

  // update the user's details
  UPDATE_USER = 12,
  UPDATE_USER_ACK = 13,
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
/**
 * @brief function used to printf the error message!
 * 
 * @param e 
 */
void logError(const sql::SQLException &e);

#endif