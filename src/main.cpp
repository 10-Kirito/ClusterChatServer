#include <iostream>
#include <nlohmann/json.hpp>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

using json = nlohmann::json;

int main() {
    const json test = {
        {"happy", true},
        {"pi", 3.141},
    };
    std::cout << test.dump(4) << std::endl;

    muduo::net::EventLoop loop;
    LOG_INFO << "muduo install test";
    return 0;
}