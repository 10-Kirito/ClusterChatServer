cmake_minimum_required(VERSION 3.31)
project(ClusterChatServer)

set(CMAKE_CXX_STANDARD 20)

# 第三方库
# - nlohmann::json
include_directories(${PROJECT_SOURCE_DIR}/third_party/nlohmann)
# - muduo
include_directories(${PROJECT_SOURCE_DIR}/third_party/muduo)
add_subdirectory(third_party/muduo)

add_executable(ClusterChatServer src/main.cpp)

target_link_libraries(ClusterChatServer muduo_net muduo_base pthread)