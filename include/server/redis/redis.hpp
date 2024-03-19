#ifndef REDIS_HPP
#define REDIS_HPP
#include <functional>
#include <hiredis/hiredis.h>
#include <string>
class Redis {
public:
  Redis();
  ~Redis();
  // connect the redis server
  bool connect();
  // publish the message to the redis server
  bool publish(int channel, const std::string &message);
  // subscribe the message from the redis server
  bool subscribe(int channel);
  // unsubscribe the message from the redis server
  bool unsubscribe(int channel);
  // receive the message from the subscribe channel, thread alone
  void observerChannelMessage();
  // initialize the callback function to send the message to the chat server
  void initNotifyHandler(std::function<void(int, std::string)> fn);

private:
  // publish the message
  redisContext *_publish_context;
  // subscribe the message
  redisContext *_subscribe_context;
  // receive the message from the subscribe channel, and send it to the chat
  // server
  std::function<void(int, std::string)> _notify_message_handler;
};
#endif // REDIS_HPP