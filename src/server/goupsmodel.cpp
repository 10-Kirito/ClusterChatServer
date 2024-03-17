#include "database.hpp"
#include "groups/groupsmodel.hpp"
#include <cppconn/prepared_statement.h>

bool GroupsModel::create(Group &group) {
  DataBase &database = DataBase::getInstance();
  std::string sql = "insert into AllGroup (groupname, groupdesc) value (?, ?);";
  DataBase::PreparedStatementPtr pstmt(
      database.getConnection()->prepareStatement(sql));
  pstmt->setString(1, group.getName());
  pstmt->setString(2, group.getDesc());

  bool result = pstmt->executeUpdate() > 0;

  if (result) {
    // set the groupid and user can use the groupid to join or quit
    std::string lastid = "select last_insert_id() as groupid;";
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
    groups.push_back(group);
  }
  return groups;
}