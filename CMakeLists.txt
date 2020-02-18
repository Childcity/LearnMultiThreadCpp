cmake_minimum_required(VERSION 3.10)
project(LearnMultiThreadC++)

set(CMAKE_CXX_STANDARD 17)

message("Project Dir is: " ${PROJECT_SOURCE_DIR})
message("Current OS is: " ${CMAKE_SYSTEM_NAME})

if (UNIX)
    set(CMAKE_CXX_FLAGS "-pthread -Wall -Wno-reorder")
endif (UNIX)


option(BUILD_SwapTrick "Enable building SwapTrick" OFF)
option(BUILD_ThreadSafeQueue "Enable building ThreadSafeQueue" OFF)
option(BUILD_ThreadSafeStack "Enable building ThreadSafeStack" OFF)
option(BUILD_ThreadSafeHashMap "Enable building ThreadSafeHashMap" ON)

if(BUILD_SwapTrick)
    add_subdirectory(SwapTrick)
endif()
if(BUILD_ThreadSafeQueue)
    add_subdirectory(ThreadSafeQueue)
endif()
if(BUILD_ThreadSafeStack)
    add_subdirectory(ThreadSafeStack)
endif()
if(BUILD_ThreadSafeHashMap)
    add_subdirectory(ThreadSafeHashMap)
endif()

