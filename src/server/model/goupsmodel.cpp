#include "database.hpp"
#include "groups/groupsmodel.hpp"
#include "user.hpp"
#include <cppconn/prepared_statement.h>
#include <vector>

bool GroupsModel::create(Group &group) {
  DataBase &database = DataBase::getInstance();
  std::string sql = "insert into AllGroup (groupname, groupdesc) value (?, ?)";
  DataBase::PreparedStatementPtr pstmt(
      database.getConnection()->prepareStatement(sql));
  pstmt->setString(1, group.getName());
  pstmt->setString(2, group.getDesc());

  bool result = pstmt->executeUpdate() > 0;

  if (result) {
    // set the groupid and user can use the groupid to join or quit
    std::string lastid = "select last_insert_id() as groupid";
    DataBase::PreparedStatementPtr pstmt_1(
        database.getConnection()->prepareStatement(lastid));
    DataBase::ResultPtr res(pstmt_1->executeQuery());
    if (res->next()) {
      group.setId(res->getInt("groupid"));
    }
  }
  return result;
}
// delete the group and the users in the group
void GroupsModel::deleteGroup(int groupid) {}
// just delete the user in the group
void quitGroup(int userid, int groupid);
// add the user to the group and set the role
void addGroup(int userid, int groupid, std::string role = "normal");

/**
 * @brief query the groups the user in:
 *
 * @param userid
 * @return std::vector<Group>
 */
std::vector<Group> GroupsModel::queryGroups(const int &userid) {
  DataBase &database = DataBase::getInstance();
  std::string sql = "select a.id, a.groupname, a.groupdesc, b.userid from "
                    "AllGroup a join GroupUser b on a.id = b.groupid join User "
                    "c on c.id = b.userid where b.userid = ?";
  DataBase::PreparedStatementPtr pstmt(
      database.getConnection()->prepareStatement(sql));
  pstmt->setInt(1, userid);
  DataBase::ResultPtr res(pstmt->executeQuery());
  std::vector<Group> groups;
  while (res->next()) {
    Group group;
    group.setId(res->getInt("id"));
    group.setName(res->getString("groupname"));
    group.setDesc(res->getString("groupdesc"));
    // get all the users in the group
    std::string sql1 = "select u.id, u.name, u.state from GroupUser g join "
                       "User u on g.userid = u.id where g.groupid = ?";
    DataBase::PreparedStatementPtr pstmt1(
        database.getConnection()->prepareStatement(sql1));
    pstmt1->setInt(1, group.getId());
    DataBase::ResultPtr res1(pstmt1->executeQuery());
    std::vector<User> users;
    while (res1->next()) {
      User user;
      user.setId(res1->getInt("id"));
      user.setName(res1->getString("name"));
      user.setState(res1->getString("state"));
      users.push_back(user);
    }
    group.setUsers(users);
    //end get all the users in the group
    groups.push_back(group);
  }
  return groups;
}

/**
 * @brief get the group and the users in the group
 *
 * @param groupid
 * @return Group
 */
Group GroupsModel::query(const int &groupid) {
  std::string sql1 = "select * from AllGroup where id = ?";
  std::string sql2 = "select b.* from GroupUser a join User b on a.userid = "
                     "b.id where groupid = ?";
  DataBase &database = DataBase::getInstance();
  DataBase::PreparedStatementPtr pstmt1(
      database.getConnection()->prepareStatement(sql1));
  DataBase::PreparedStatementPtr pstmt2(
      database.getConnection()->prepareStatement(sql2));
  pstmt1->setInt(1, groupid);
  pstmt2->setInt(1, groupid);
  DataBase::ResultPtr res1(pstmt1->executeQuery());
  DataBase::ResultPtr res2(pstmt2->executeQuery());
  Group group;
  if (res1->next()) {
    group.setId(res1->getInt("id"));
    group.setName(res1->getString("groupname"));
    group.setDesc(res1->getString("groupdesc"));
  }
  std::vector<User> users;
  while (res2->next()) {
    User user;
    user.setId(res2->getInt("id"));
    user.setName(res2->getString("name"));
    user.setState(res2->getString("state"));
    users.push_back(user);
  }
  group.setUsers(users);
  return group;
}