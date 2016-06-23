project(color_connection)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(LED_SOURCE led.cpp)

if(WIN32)
else()
    set(LED_SOURCE ${LED_SOURCE} pi/ledctrl.cpp)
endif()

add_executable(color_connection ${LED_SOURCE})

target_link_libraries(color_connection nearby)

