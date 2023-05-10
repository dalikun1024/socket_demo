cmake_minimum_required(VERSION 3.10)

project(socket_c_client C)

set(CMAKE_GENERATOR "Unix Makefiles")

set(SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/client.c)

add_executable(client ${SRC_LIST})

