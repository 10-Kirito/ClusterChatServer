cmake_minimum_required(VERSION 3.10)

project(chat-server)

# set cxx compile flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# set the executable output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set the search path of header files
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/include/server/models)
include_directories(${PROJECT_SOURCE_DIR}/include/server/models/user)
include_directories(${PROJECT_SOURCE_DIR}/include/server/models/friends)
include_directories(${PROJECT_SOURCE_DIR}/include/server/models/groups)
include_directories(${PROJECT_SOURCE_DIR}/include/server/models/message)
include_directories(${PROJECT_SOURCE_DIR}/include/client)
include_directories(${PROJECT_SOURCE_DIR}/include/tools)
include_directories(${PROJECT_SOURCE_DIR}/thirdpart)

# load subdirectory
add_subdirectory(src)