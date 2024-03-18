aux_source_directory(. SRC_LIST)

add_executable(chatclient ${SRC_LIST})

target_link_libraries(chatclient pthread)

include_directories(${PROJECT_SOURCE_DIR}/src/client/include)