cmake_minimum_required(VERSION 3.14)
project(echo)

set(CMAKE_CXX_STANDARD 11)

add_executable(echo_server server.cpp)
add_executable(echo_client client.cpp)