# CMakeLists.txt
aux_source_directory(./tools TOOLS_LIST)
# generate static library for tools/*
add_library(tools STATIC ${TOOLS_LIST} )

target_link_libraries(tools PUBLIC mysqlcppconn)

add_subdirectory(server)