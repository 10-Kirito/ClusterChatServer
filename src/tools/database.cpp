#include "database.hpp"
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <memory>
#include <muduo/base/Logging.h>
#include "public.hpp"


DataBase::DataBase() : DataBase("127.0.0.1:3306", "kirito", "Zpf.123456") {}
/**
 * @brief Construct a new Data Base:: Data Base object
 *
 * @param iport
 * @param username
 * @param password
 */
DataBase::DataBase(const std::string &iport, const std::string &username,
                   const std::string &password) {

  // Attention!!!!
  // The _driver's resource is acquired by the get_driver_instance() function.
  // The function is static, so we don't need to delete the _driver's resource.
  try {
    _driver = std::shared_ptr<sql::Driver>(get_driver_instance(),
                                           [](sql::Driver *) {});
    // create a connection, the resource is acquired by bew(), so we should
    // delete
    _connection = std::shared_ptr<sql::Connection>(
        _driver->connect(iport, username, password),
        [](sql::Connection *conn) { delete conn; });

    if (_connection && !_connection->isClosed()) {
      LOG_INFO << "(From DataBase: Connect to the database: " << iport
               << " successfully!)";
    } else {
      LOG_INFO << "Failed to connect to the database: " << iport;
    }

    // set the database:
    _connection->setSchema("chatserver");

  // create a statement and store it in the shared_ptr
    /* _statement = std::shared_ptr<sql::Statement>(
        _connection->createStatement(),
        [](sql::Statement *state) { delete state; });
 */  } catch (sql::SQLException &e) {
      logError(e);
    }
}

/**
 * @brief acquire a PreparedStatementPtr
 *
 * @param sql
 * @return DataBase::PreparedStatementPtr
 */
DataBase::PreparedStatementPtr
DataBase::getPreStatement(const std::string &sql) {
  return std::shared_ptr<sql::PreparedStatement>(
      _connection->prepareStatement(sql),
      [](sql::PreparedStatement *state) { delete state; });
}

/**
 * @brief update MYSQL database
 *
 * @param sql
 * @return true
 * @return false
 */
bool DataBase::update(const std::string &sql) {
  bool flag;
  try {
    StatementPtr statement(_connection->createStatement());
    flag = statement->execute(sql);
  } catch (sql::SQLException &e) {
    logError(e);
  }
  return flag;
}

bool DataBase::update(DataBase::PreparedStatementPtr pstmt) {
  return pstmt->executeUpdate() > 0;
}

DataBase::ResultPtr DataBase::query(const std::string &sql) {
  DataBase::ResultPtr result;
  try {
    StatementPtr statement(_connection->createStatement());
    result = std::shared_ptr<sql::ResultSet>(statement->executeQuery(sql));
  } catch (sql::SQLException &e) {
    logError(e);
  }
  return result;
}
