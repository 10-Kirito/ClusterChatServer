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
                    "msgtype) value (?, ?, ?, ?);";
  DataBase &database = DataBase::getInstance();
  std::shared_ptr<sql::PreparedStatement> pstmt(
      database.getConnection()->prepareStatement(sql));
  // set the parameters:
  pstmt->setInt(1, msg.fromId);
  pstmt->setString(2, msg.message);
  pstmt->setInt(3, msg.toId);
  pstmt->setInt(4, msg.msgtype);
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
  std::string sql = "DELETE FROM OfflineMessage WHERE fromId = ?;";
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
                           result->getInt("msgtype")});
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