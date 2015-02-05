project(NXE CXX)
cmake_minimum_required(VERSION 2.8.11)

add_definitions(-std=c++11)

add_subdirectory(src)

if(NOT DISABLE_TESTS)
    add_subdirectory(tests)
endif()
