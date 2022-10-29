cmake_minimum_required(VERSION 3.16)
project(a2 C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
add_executable(untitled main.c queue.c queue.h)
