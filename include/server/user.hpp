#ifndef USER_H
#define USER_H

#include <string>

using std::string;
/**
 * @brief The User Class
 * 
 */
class User {
private:
  int _id;
  string _name;
  string _password;
  string _state;

public:
  User(int id = -1, string name = "", string pwd = "", string state = "offline")
      : _id(id), _name(name), _password(pwd), _state(state) {}

  inline void setId(int id) { _id = id; }
  inline void setName(string name) { _name = name; }
  inline void setPwd(string pwd) { _password = pwd; }
  inline void setState(string state) { _state = state; }

  inline int getId () const { return _id; }
  inline string getName() const { return _name; }
  inline string getPwd() const { return _password; }
  inline string getState() const { return _state; }
};

#endif // USER_H