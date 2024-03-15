/**
 * @file database.hpp
 * @author 10-Kirito (10.z.p.f.kirito@gmail.com)
 * @brief this file defines the class to connect the database.
 * @version 0.1
 * @date 2024-03-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef DATABASE_H
#define DATABASE_H
#include "singletontemplate.hpp"
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <memory>
#include <string>
/**
 * @brief the class to connect to the database
 *
 */
class DataBase : public SingletonTemplate<DataBase> {
  friend class SingletonTemplate<DataBase>;

public:
  typedef std::shared_ptr<sql::Statement> StatementPtr;
  typedef std::shared_ptr<sql::PreparedStatement> PreparedStatementPtr;
  typedef std::shared_ptr<sql::ResultSet> ResultPtr;

private:
  typedef std::shared_ptr<sql::Driver> DriverPtr;
  typedef std::shared_ptr<sql::Connection> ConnectionPtr;
  DataBase();
  DataBase(const std::string &, const std::string &, const std::string &);
  DataBase(const DataBase &) = delete;
  DataBase &operator=(const DataBase &) = delete;
  DriverPtr _driver;
  ConnectionPtr _connection;

public:
  PreparedStatementPtr getPreStatement(const std::string &);
  inline ConnectionPtr getConnection() { return _connection; }
  // the funtion to use the database
  bool update(const std::string &); // insert new data and update the data
  bool update(PreparedStatementPtr);
  ResultPtr query(const std::string &); // query the data
  ResultPtr query(PreparedStatementPtr);
  bool deletedata(const std::string &);  // delete the data
  bool deletedata(PreparedStatementPtr); // delete the data
};
#endif // DATABASE_H