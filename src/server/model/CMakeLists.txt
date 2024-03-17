# CMakeLists.txt
aux_source_directory(. MODEL_LIST)

# generate static library for tools/*
add_library(model STATIC ${MODEL_LIST})

target_link_libraries(model PUBLIC mysqlcppconn tools)
