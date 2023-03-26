cmake_minimum_required(VERSION 3.24)
project(merge_sort)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS -pthread)

find_package(Threads REQUIRED)
include_directories(.)

add_executable(merge_sort
        main.cpp)
