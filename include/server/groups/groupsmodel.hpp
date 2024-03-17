#ifndef GROUPS_MODEL_H
#define GROUPS_MODEL_H
#include "group.hpp"

class GroupsModel {
public:
  bool create(Group &group);
  // delete the group and the users in the group
  void deleteGroup(int groupid);
  // just delete the user in the group
  void quitGroup(int userid, int groupid);
  // add the user to the group and set the role
  void addGroup(int userid, int groupid, std::string role = "normal");
  // the group if exists
  bool exist(const int &group_id);
  // query the groups of the user
  std::vector<Group> queryGroups(const int &);

private:
};

#endif // GROUPS_MODEL_H