#ifndef GROUP_USER_MODEL_H
#define GROUP_USER_MODEL_H

#include "groups/groupuser.hpp"
class GroupUserModel {
public:
  bool insert(const GroupUser &groupUser);
};
#endif // GROUP_USER_MODEL_H