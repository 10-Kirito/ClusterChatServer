#include "groups/groupusermodel.hpp"
#include "database.hpp"
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

/**
 * @brief query the user in the group
 *
 * @param userid
 * @param groupid
 * @return GroupUser
 */
GroupUser GroupUserModel::query(int userid, int groupid) {
  DataBase &database = DataBase::getInstance();
  std::string sql = "select * from GroupUser where userid = ? and groupid = ?";
  DataBase::PreparedStatementPtr pstmt(
      database.getConnection()->prepareStatement(sql));
  pstmt->setInt(1, userid);
  pstmt->setInt(2, groupid);

  DataBase::ResultPtr res(pstmt->executeQuery());

  if (res->next()) {
    GroupUser groupUser;
    groupUser.setGroupid(res->getInt("groupid"));
    groupUser.setUserid(res->getInt("userid"));
    groupUser.setRole(res->getString("role"));
    return groupUser;
  } else {
    return GroupUser();
  }
}

/**
 * @brief delete all users in the group
 *
 * @param groupid
 * @return true
 * @return false
 */
bool GroupUserModel::deleteAll(const int &groupid) {
  DataBase &database = DataBase::getInstance();
  std::string sql = "delete from GroupUser where groupid = ?";
  DataBase::PreparedStatementPtr pstmt(
      database.getConnection()->prepareStatement(sql));
  pstmt->setInt(1, groupid);

  bool result = pstmt->executeUpdate() > 0;

  return result;
}

/**
 * @brief delete the user from the group
 *
 * @param userid
 * @param groupid
 * @return true
 * @return false
 */
bool GroupUserModel::deleteOne(const int &userid, const int &groupid) {
  DataBase &database = DataBase::getInstance();
  std::string sql = "delete from GroupUser where groupid = ? and userid = ?";
  DataBase::PreparedStatementPtr pstmt(
      database.getConnection()->prepareStatement(sql));
  pstmt->setInt(1, groupid);
  pstmt->setInt(2, userid);

  bool result = pstmt->executeUpdate() > 0;

  return result;
}
