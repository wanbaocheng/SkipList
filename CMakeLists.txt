cmake_minimum_required(VERSION 3.12)
#set(CMAKE_C_COMPILER /usr/bin/gcc-7)
#set(CMAKE_CXX_COMPILER /usr/bin/g++-7)
project(skiplist)
set(CMAKE_CXX_STANDARD 11)

add_executable(test test.cc)
target_link_libraries(test)