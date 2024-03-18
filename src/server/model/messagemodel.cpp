#include "message/messagemodel.hpp"
#include "database.hpp"
#include "message/messge.hpp"
#include "public.hpp"
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <memory>
#include <muduo/base/Logging.h>
#include <vector>
/**
 * @brief insert a message into the database
 *
 * @param msg
 */
void MessageModel::insert(const Message &msg) {
  std::string sql = "INSERT INTO OfflineMessage(fromId, message, toId, "
                    "msgtype, time) value (?, ?, ?, ?, ?);";
  DataBase &database = DataBase::getInstance();
  std::shared_ptr<sql::PreparedStatement> pstmt(
      database.getConnection()->prepareStatement(sql));
  // set the parameters:
  pstmt->setInt(1, msg.fromId);
  pstmt->setString(2, msg.message);
  pstmt->setInt(3, msg.toId);
  pstmt->setInt(4, static_cast<int>(msg.msgtype));
  pstmt->setString(5, msg.time);
  // execute the sql
  bool success = database.update(pstmt);
  // log the info
  LOG_INFO << "--EXcute the SQL statement " << sql << "status..."
           << (success ? "success" : "failed");
}

/**
 * @brief delete the message
 *
 * @param msg
 */
void MessageModel::deleteMsg(const Message &msg) {
  std::string sql = "DELETE FROM OfflineMessage WHERE fromId = ?";
  DataBase &database = DataBase::getInstance();
  std::shared_ptr<sql::PreparedStatement> pstmt(
      database.getConnection()->prepareStatement(sql));
  // set the parameters:
  pstmt->setInt(1, msg.fromId);
  // execute the sql
  try {
    bool success = pstmt->execute();
    // log the information
    LOG_INFO << "--EXcute the SQL statement " << sql << "status..."
             << (success ? "success" : "failed");
  } catch (const sql::SQLException &e) {
    logError(e);
  }
}

void MessageModel::deleteGroupMsg(const Message &msg) {
  std::string sql = "DELETE FROM OfflineMessage WHERE fromId = ? and toId = ? "
                    "and message = ? and msgtype = 5 and time = ? limit 1";
  DataBase &database = DataBase::getInstance();
  std::shared_ptr<sql::PreparedStatement> pstmt(
      database.getConnection()->prepareStatement(sql));
  // set the parameters:
  pstmt->setInt(1, msg.fromId);
  pstmt->setInt(2, msg.toId);
  pstmt->setString(3, msg.message);
  pstmt->setString(4, msg.time);
  // execute the sql
  try {
    bool success = pstmt->execute();
    // log the information
    LOG_INFO << "--EXcute the SQL statement " << sql << "status..."
             << (success ? "success" : "failed");
  } catch (const sql::SQLException &e) {
    logError(e);
  }
}

/**
 * @brief query the offline messages
 *
 * @param id
 * @return std::vector<Message>
 */
std::vector<Message> MessageModel::queryMsg(int id) {
  std::vector<Message> resultSet;

  std::string sql =
      "select * from OfflineMessage WHERE OfflineMessage.toId = ?";
  DataBase &database = DataBase::getInstance();
  std::shared_ptr<sql::PreparedStatement> pstmt(
      database.getConnection()->prepareStatement(sql));
  // set the parameters:
  pstmt->setInt(1, id);
  // execute the sql
  try {
    DataBase::ResultPtr result(pstmt->executeQuery());

    while (result->next()) {
      resultSet.push_back({result->getInt("fromId"), result->getInt("toId"),
                           result->getString("message"),
                           static_cast<MessageType>(result->getInt("msgtype")),
                           result->getString("time")});
    }
    return resultSet;
  } catch (const sql::SQLException &e) {
    logError(e);
    return std::vector<Message>();
  }
}

/**
 * @brief query the offline message in groups
 *
 * @param groupid
 * @return std::vector<Message>
 */
std::vector<Message> MessageModel::queryGroupMsg(int groupid, int userid) {
  std::vector<Message> resultSet;
  std::string sql = "select distinct * from OfflineMessage where "
                    "OfflineMessage.toId = ? and OfflineMessage.msgtype = 5 "
                    "and OfflineMessage.fromId != ?";
  DataBase &database = DataBase::getInstance();
  std::shared_ptr<sql::PreparedStatement> pstmt(
      database.getConnection()->prepareStatement(sql));
  // set the parameters:
  pstmt->setInt(1, groupid);
  pstmt->setInt(2, userid);
  // execute the sql
  try {
    DataBase::ResultPtr result(pstmt->executeQuery());

    while (result->next()) {
      resultSet.push_back({result->getInt("fromId"), result->getInt("toId"),
                           result->getString("message"),
                           static_cast<MessageType>(result->getInt("msgtype")),
                           result->getString("time")});
    }
    return resultSet;
  } catch (const sql::SQLException &e) {
    logError(e);
    return std::vector<Message>();
  }
}

/**
 * @brief delete the messages
 *
 * @param list
 */
void MessageModel::deleteMsg(const std::vector<Message> &list) {
  for (const Message &msg : list) {
    deleteMsg(msg);
  }
}
void MessageModel::deleteGroupMsg(const std::vector<Message> &list) {
  for (const Message &msg : list) {
    deleteGroupMsg(msg);
  }
}