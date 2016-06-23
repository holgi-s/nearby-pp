project(nearby)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(LIB_SOURCE NearbyServer.cpp NearbySession.cpp NearbyMessage.cpp ProtoBuf.cpp Discover.cpp)

if(WIN32)
    set(LIB_SOURCE ${LIB_SOURCE} win32/Bonjour.cpp)
else()
    set(LIB_SOURCE ${LIB_SOURCE} pi/Avahi.cpp)
endif()

add_library(nearby ${LIB_SOURCE})

if(WIN32)
    target_link_libraries(nearby ws2_32)
else()
    target_link_libraries(nearby avahi-client avahi-common pthread)
endif()


