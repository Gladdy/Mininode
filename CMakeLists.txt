cmake_minimum_required (VERSION 2.8.11)
project (NodeDemystified)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory (mininode)

include_directories(mininode)
add_executable (server server.cpp)

target_link_libraries(server TCPServer)
