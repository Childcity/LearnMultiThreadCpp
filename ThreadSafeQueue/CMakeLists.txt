cmake_minimum_required(VERSION 3.10)
project(ThreadSafeQueue)

add_executable(ThreadSafeQueue
        StdThreadSafeQueue.hpp
        MyThreadSafeQueue.hpp
        main.cpp)
