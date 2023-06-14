cmake_minimum_required(VERSION 3.22)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 14)

add_definitions("-g -Os -Wall -fPIC -std=c++14")

file(GLOB SRC_FILES
        "${PROJECT_SOURCE_DIR}/src/*")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

#add_library(cm STATIC ${SRC_FILES})

add_executable(ThreadPool main.cpp ${SRC_FILES})

find_package(Threads)