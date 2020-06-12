cmake_minimum_required(VERSION 3.10)
project(ThreadSafeStack)

add_executable(ThreadSafeStack
        StdThreadSafeStack.hpp
        main.cpp)