#ifndef FRIENDSMODEL_H
#define FRIENDSMODEL_H
#include "friends.hpp"
#include "user.hpp"
#include <vector>
class FriendModel {
public:
  void insert(const Friends &friends);
  void deleteFriend(int userid, int friendid);
  std::vector<User> query(const int &);
};
#endif // FRIENDSMODEL_H