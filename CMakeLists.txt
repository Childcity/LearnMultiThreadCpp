cmake_minimum_required(VERSION 3.10)
project(LearnMultiThreadC++)

set(CMAKE_CXX_STANDARD 17)

message("Project Dir is: " ${PROJECT_SOURCE_DIR})
message("Current OS is: " ${CMAKE_SYSTEM_NAME})

if (UNIX)
    set(CMAKE_CXX_FLAGS "-pthread -Wall -Wno-reorder")
endif (UNIX)


option(BUILD_SwapTrick "Enable building SwapTrick" ON)
option(BUILD_ThreadSafeQueue "Enable building ThreadSafeQueue" ON)
option(BUILD_ThreadSafeStack "Enable building ThreadSafeStack" ON)
option(BUILD_ThreadSafeHashMap "Enable building ThreadSafeHashMap" ON)
option(BUILD_ThreadSafeList "Enable building ThreadSafeList" ON)

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
if(BUILD_ThreadSafeList)
    add_subdirectory(ThreadSafeList)
endif()

