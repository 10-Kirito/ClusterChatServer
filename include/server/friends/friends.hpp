#ifndef FRIENDS_H
#define FRIENDS_H
/**
 * @brief The Friends Class
 * 
 */
class Friends {
  int userid;
  int friendid;
public:
  Friends(int userid, int friendid) : userid(userid), friendid(friendid) {}
  inline int getUserid() const { return userid; }
  inline void setUserid(int userid) { Friends::userid = userid; }
  inline int getFriendid() const { return friendid; }
  inline void setFriendid(int friendid) { Friends::friendid = friendid; }
};

#endif // FRIENDS_H