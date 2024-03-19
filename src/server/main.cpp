#include "chatserver.hpp"
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " ip: port" << std::endl;
  }
  // input the IP and PORT:
  char *ip = argv[1];
  uint16_t port = std::stoi(argv[2]);
  EventLoop loop;
  InetAddress addr(ip, port);
  ChatServer server(&loop, addr, "ChatServer");

  server.start();
  loop.loop();

  return 0;
}