cmake_minimum_required(VERSION 3.16)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 17)

add_executable(ThreadPool main.cpp)

find_package(Threads)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})