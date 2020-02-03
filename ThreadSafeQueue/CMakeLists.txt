cmake_minimum_required(VERSION 3.10)
project(ThreadSafeQueue)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread -Wall -Wno-reorder")

add_executable(ThreadSafeQueue main.cpp)
