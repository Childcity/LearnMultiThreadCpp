cmake_minimum_required(VERSION 3.10)
project(ThreadSafeStack)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread -Wall -Wno-reorder")

add_executable(ThreadSafeStack main.cpp)