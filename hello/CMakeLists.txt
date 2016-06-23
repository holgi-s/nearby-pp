project(hello)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(HELLO_SOURCE main.cpp)

add_executable(hello ${HELLO_SOURCE})
target_link_libraries(hello nearby)

