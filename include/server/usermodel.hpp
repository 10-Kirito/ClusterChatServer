#ifndef USERMODEL_H
#define USERMODEL_H
#include "user.hpp"

class UserModel {
public:
  bool insert(User &user);
  bool update(User &user);
  User query(const int &);
  bool exist(const int &);
};

#endif // USERMODEL_H