#include "usermodel.hpp"
#include "database.hpp"
#include "public.hpp"
#include "user.hpp"
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <muduo/base/Logging.h>

/**
 * @brief create a new user and set the user's id
 *
 * @param user
 * @return true
 * @return false
 */
bool UserModel::insert(User &user) {
  try {
    // LOG_INFO << "--Execute the SQL to insert a new user...";
    std::string sql = "INSERT INTO User(name, password) VALUES(?,?)";
    // get the database instance
    DataBase &database = DataBase::getInstance();
    // get the PreparedStatementPtr
    std::unique_ptr<sql::PreparedStatement> pstmt(
        database.getConnection()->prepareStatement(sql));
    // set the parameters:
    pstmt->setString(1, user.getName());
    pstmt->setString(2, user.getPwd());
    // execute the sql
    bool success = pstmt->executeUpdate() >= 0;
    LOG_INFO << "--Execute the SQL statement " << sql << "status..."
             << (success ? "success" : "failed");
    if (success) {
      // get the user's id
      std::string sql_id = "select LAST_INSERT_ID()";
      DataBase::ResultPtr result = database.query(sql_id);
      if (result->next()) {
        LOG_INFO << "--Execute the SQL statement " << sql_id << "status..."
                 << "success";
        user.setId(result->getInt(1));
        return true;
      } else {
        LOG_INFO << "--Execute the SQL statement " << sql_id << "status..."
                 << "failed";
        return false;
      }
    } else {
      return false;
    }
  } catch (sql::SQLException &e) {
    logError(e);
    return false;
  }
}

/**
 * @brief This is used to query the user by the user's id
 *
 * @param id
 * @return User
 */
User UserModel::query(const int &id) {
  User user;
  try {
    std::string sql = "select * from User where id = ?";
    DataBase &database = DataBase::getInstance();
    DataBase::PreparedStatementPtr pstmt(
        database.getConnection()->prepareStatement(sql));

    pstmt->setInt(1, id);
    DataBase::ResultPtr result(pstmt->executeQuery());

    LOG_INFO << "--Execute the SQL statement " << sql << "status..."
             << (result->rowsCount() > 0 ? "success" : "failed");
    if (result->next()) {
      user.setId(result->getInt("id"));
      user.setName(result->getString("name"));
      user.setPwd(result->getString("password"));
      user.setState(result->getString("state"));
    }
    return user;

  } catch (sql::SQLException &e) {
    logError(e);
    return user;
  }
}

/**
 * @brief update the user's details
 *
 * @param user
 * @return true
 * @return false
 */
bool UserModel::update(User &user) {
  try {
    std::string sql = "update User set state = ?, name = ?, "
                      "password = ?  where id = ?;";
    DataBase &database = DataBase::getInstance();

    DataBase::PreparedStatementPtr pstmt(
        database.getConnection()->prepareStatement(sql));

    pstmt->setString(1, user.getState());
    pstmt->setString(2, user.getName());
    pstmt->setString(3, user.getPwd());
    pstmt->setInt(4, user.getId());

    bool result = pstmt->executeUpdate() >= 0;
    LOG_INFO << "--Execute the SQL statement" << sql << "status..."
             << (result ? "success" : "failed");

    return result;
  } catch (sql::SQLException &e) {
    logError(e);
    return false;
  }
}