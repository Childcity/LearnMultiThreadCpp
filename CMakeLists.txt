cmake_minimum_required(VERSION 3.10)
project(LearnMultiThreadC++)

option(BUILD_ThreadSafeQueue "Enable building ThreadSafeQueue" ON)
option(BUILD_ThreadSafeStack "Enable building ThreadSafeStack" ON)

if(BUILD_ThreadSafeQueue)
    add_subdirectory(ThreadSafeQueue)
endif()
if(BUILD_ThreadSafeStack)
    add_subdirectory(ThreadSafeStack)
endif()

