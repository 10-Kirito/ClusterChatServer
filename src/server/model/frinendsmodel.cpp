#include "database.hpp"
#include "friends/friends.hpp"
#include "friends/friendsmodel.hpp"
#include "public.hpp"
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <memory>
#include <muduo/base/Logging.h>
#include <string>
#include <vector>

void FriendModel::insert(const Friends &friends) {
  // insert the friend relationship into the database
  std::string sql = "insert into Friend(userid, friendid) value(?, ?)";
  std::string sql_1 = "insert into Friend(userid, friendid) value(?, ?)";
  DataBase &database = DataBase::getInstance();
  std::shared_ptr<sql::PreparedStatement> pstmt(
      database.getConnection()->prepareStatement(sql));
  std::shared_ptr<sql::PreparedStatement> pstmt_1(
      database.getConnection()->prepareStatement(sql_1));

  pstmt->setInt(1, friends.getUserid());
  pstmt->setInt(2, friends.getUserid());

  pstmt_1->setInt(2, friends.getUserid());
  pstmt_1->setInt(1, friends.getUserid());

  bool success;
  try {
    success = database.update(pstmt);
    success = database.update(pstmt_1) && success;
  } catch (sql::SQLException &e) {
    logError(e);
  }
  LOG_INFO << "--Add the friends: "
           << "status..." << (success ? "success" : "failed");
}

std::vector<User> FriendModel::query(const int &userid) {
  std::string sql =
      "select u.id, u.name , u.state from Friend f join User u on "
      "f.friendid = u.id where userid = ?";

  DataBase &database = DataBase::getInstance();
  std::shared_ptr<sql::PreparedStatement> pstmt(
      database.getConnection()->prepareStatement(sql));
  pstmt->setInt(1, userid);

  std::vector<User> friends;

  DataBase::ResultPtr result = database.query(pstmt);
  while (result->next()) {
    friends.push_back(User(result->getInt("id"), result->getString("name"),
                           result->getString("state")));
  }

  return friends;
}
