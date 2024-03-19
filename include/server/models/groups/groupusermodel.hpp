#ifndef GROUP_USER_MODEL_H
#define GROUP_USER_MODEL_H

#include "groups/groupuser.hpp"
class GroupUserModel {
public:
  bool insert(const GroupUser &groupUser);
  bool deleteAll(const int &groupid);
  bool deleteOne(const int &userid, const int &groupid);
  GroupUser query(int userid, int groupid);
};
#endif // GROUP_USER_MODEL_H