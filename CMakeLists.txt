cmake_minimum_required(VERSION 3.8)
project(Pool)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Flatten.h Future.h Map.h Promise.h SharedState.h ThreadPool.cpp ThreadPool.h)
add_executable(Pool ${SOURCE_FILES})