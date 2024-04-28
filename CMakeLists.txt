cmake_minimum_required(VERSION 3.10)
project(NetworkProject C)

set(CMAKE_C_STANDARD 99)

add_executable(client src/client.c)
add_executable(server src/server.c)
add_executable(ipshow src/ipshow.c)