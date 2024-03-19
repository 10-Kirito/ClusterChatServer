#include "redis.hpp"
#include <hiredis/hiredis.h>
#include <hiredis/read.h>
#include <muduo/base/Logging.h>
#include <thread>

Redis::Redis() : _publish_context(nullptr), _subscribe_context(nullptr) {}

Redis::~Redis() {
  if (_publish_context != nullptr) {
    redisFree(_publish_context);
  }
  if (_subscribe_context != nullptr) {
    redisFree(_subscribe_context);
  }
}

bool Redis::connect() {
  _publish_context = redisConnect("127.0.0.1", 6379);

  if (nullptr == _publish_context) {
    LOG_INFO << "connect redis failed";
    return false;
  }

  _subscribe_context = redisConnect("127.0.0.1", 6379);
  if (nullptr == _subscribe_context) {
    LOG_INFO << "connect redis failed";
    return false;
  }

  std::thread t([&]() { observerChannelMessage(); });
  t.detach();
  LOG_INFO << "connect redis-server success!";
  return true;
}

bool Redis::publish(int channel, const std::string &message) {
  redisReply *reply = (redisReply *)redisCommand(
      _publish_context, "PUBLISH %d %s", channel, message.c_str());

  if (nullptr == reply) {
    LOG_INFO << "publish message failed";
    return false;
  }
  freeReplyObject(reply);
  return true;
}

bool Redis::subscribe(int channel) {
  // the redisAppendCommand() function is asynchronous, and it will return
  // immediately after the command is sent to the server
  if (REDIS_ERR ==
      redisAppendCommand(this->_subscribe_context, "SUBSCRIBE %d", channel)) {
    LOG_INFO << "subscribe channel failed";
    return false;
  }

  // then we need to call the redisBufferWrite function to send the command to
  // the server
  int done = 0;
  while (!done) {
    if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done)) {
      LOG_INFO << "subscribe channel failed";
      return false;
    }
  }

  return true;
}

bool Redis::unsubscribe(int channel) {
  if (REDIS_ERR ==
      redisAppendCommand(this->_subscribe_context, "UNSUBSCRIBE %d", channel)) {
    LOG_INFO << "unsubscribe channel failed";
    return false;
  }

  int done = 0;
  while (!done) {
    if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done)) {
      LOG_INFO << "unsubscribe channel failed";
      return false;
    }
  }

  return true;
}

void Redis::observerChannelMessage() {
  redisReply *reply = nullptr;
  while (REDIS_OK == redisGetReply(this->_subscribe_context, (void **)&reply)) {
    // the reply will be three part:
    /*  127.0.0.1:6379> SUBSCRIBE testchannel
        1) "subscribe"
        2) "testchannel"
        3) (integer) 1
        // ------------------------------
        1) "message"
        2) "testchannel"
        3) "hello,world"
        Reading messages... (press Ctrl-C to quit or any key to type command)
    */
    if (reply != nullptr && reply->element[2] != nullptr &&
        reply->element[2]->str != nullptr) {
      // send the message to the chat server: (userid:message)
      _notify_message_handler(atoi(reply->element[1]->str),
                              reply->element[2]->str);
    }
    freeReplyObject(reply);
  }
  LOG_INFO << "observerChannelMessage quit";
}

void Redis::initNotifyHandler(std::function<void(int, std::string)> fn) {
  _notify_message_handler = fn;
}

