#ifndef USER_H
#define USER_H

#include <json.hpp>
#include <string>
using json = nlohmann::json;

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
  string _role;

public:
  User(int id = -1, string name = "", string state = "offline", string pwd = "",
       string role = "")
      : _id(id), _name(name), _password(pwd), _state(state), _role(role) {}

  inline void setId(int id) { _id = id; }
  inline void setName(string name) { _name = name; }
  inline void setPwd(string pwd) { _password = pwd; }
  inline void setState(string state) { _state = state; }
  inline void setRole(string role) { _role = role; }

  inline int getId() const { return _id; }
  inline string getName() const { return _name; }
  inline string getPwd() const { return _password; }
  inline string getState() const { return _state; }
  inline string getRole() const { return _role; }
};

inline void to_json(json &j, const User &user) {
  j = json{{"id", user.getId()},
           {"name", user.getName()},
           {"state", user.getState()},
           {"role", user.getRole()}};
}

inline void from_json(const json &j, User &p) {
  p.setId(j.at("id"));
  p.setName(j.at("name"));
  p.setState(j.at("state"));
  if(j.find("role") != j.end()) {
    p.setRole(j.at("role"));
  }
}

#endif // USER_H