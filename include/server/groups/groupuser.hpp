#ifndef GROUP_USER_H
#define GROUP_USER_H

#include <json.hpp>
#include <string>

using json = nlohmann::json;
class GroupUser {
public:
  // constructor:
  GroupUser(const int &groupid = -1, const int &userid = -1,
            const std::string &role = "normal")
      : groupid(groupid), userid(userid), role(role) {}
  // sets and gets:
  void setGroupid(const int &groupid) { this->groupid = groupid; }
  void setUserid(const int &userid) { this->userid = userid; }
  void setRole(const std::string &role) { this->role = role; }
  int getGroupid() const { return groupid; }
  int getUserid() const { return userid; }
  std::string getRole() const { return role; }

private:
  int groupid;
  int userid;
  std::string role; // ("creator" or "normal")
};

inline void to_json(json &j, const GroupUser &p) {
  j = json{{"groupid", p.getGroupid()},
           {"userid", p.getUserid()},
           {"role", p.getRole()}};
}

inline void from_json(const json &j, GroupUser &p) {
  p.setGroupid(j.at("groupid").get<int>());
  p.setUserid(j.at("userid").get<int>());
  p.setRole(j.at("role").get<std::string>());
}

#endif