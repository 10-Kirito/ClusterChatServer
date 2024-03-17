#include "groups/groupusermodel.hpp"
#include "database.hpp"
#include "groups/groupsmodel.hpp"
#include "groups/groupuser.hpp"
#include "public.hpp"
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

bool GroupUserModel::insert(const GroupUser &groupUser) {
  try {
    DataBase &database = DataBase::getInstance();
    std::string sql =
        "insert into GroupUser(groupid, userid, role) value (?, ?, ?);";
    DataBase::PreparedStatementPtr pstmt(
        database.getConnection()->prepareStatement(sql));
    pstmt->setInt(1, groupUser.getGroupid());
    pstmt->setInt(2, groupUser.getUserid());
    pstmt->setString(3, groupUser.getRole());

    bool result = pstmt->executeUpdate() > 0;
    return result;
  } catch (const sql::SQLException &e) {
    logError(e);
    return false;
  }
}

bool GroupsModel::exist(const int &group_id) {
  std::string sql = "select * from AllGroup where id = ?";
  DataBase &database = DataBase::getInstance();
  DataBase::PreparedStatementPtr pstmt(
      database.getConnection()->prepareStatement(sql));
  pstmt->setInt(1, group_id);
  DataBase::ResultPtr res(pstmt->executeQuery());
  return res->next();
}