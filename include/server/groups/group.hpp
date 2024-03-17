#ifndef GROUP_H
#define GROUP_H
#include "groups/groupuser.hpp"
#include "user.hpp"
#include <json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

class Group {
public:
  Group(const std::string &name = "", const std::string &desc = "",
        const int &id = -1)
      : id(id), name(name), desc(desc) {}
  void setId(const int &id) { this->id = id; }
  void setName(const std::string &name) { this->name = name; }
  void setDesc(const std::string &desc) { this->desc = desc; }
  void setUsers(const std::vector<GroupUser> &users) { this->users = users; }

  int getId() const { return id; }
  std::string getName() const { return name; }
  std::string getDesc() const { return desc; }
  std::vector<GroupUser> getUsers() const { return users; }

private:
  int id;
  std::string name;
  std::string desc;
  std::vector<GroupUser> users;
};

inline void to_json(json &j, const Group &p) {
  j = json{{"id", p.getId()},
           {"name", p.getName()},
           {"desc", p.getDesc()},
           {"users", p.getUsers()}};
}

inline void from_json(const json &j, Group &p) {
  p.setId(j.at("id").get<int>());
  p.setName(j.at("name").get<std::string>());
  p.setDesc(j.at("desc").get<std::string>());
  p.setUsers(j.at("users").get<std::vector<GroupUser>>());
}

#endif // GROUP_H