set(TARGET chatserver)  


# include all files in the current directory
aux_source_directory(. SRC_LIST)

# add the executable
add_executable(${TARGET} ${SRC_LIST} ../public.cpp)

# link muduo libraries
target_link_libraries(${TARGET} muduo_net muduo_base pthread)