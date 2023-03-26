cmake_minimum_required(VERSION 3.24)
project(factorial_processes)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS -pthread)

find_package(Threads REQUIRED)
add_executable(factorial_processes
        factorial.cpp)
