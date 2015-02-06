project(NXE CXX)
cmake_minimum_required(VERSION 2.8.11)

include_directories(${CMAKE_SOURCE_DIR}/3rdParty/boost-process)

add_definitions(-std=c++11)

# 3rd party
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/boost-dbus)

if(NOT DISABLE_TESTS)
    add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/gmock)
    include_directories(${CMAKE_SOURCE_DIR}/3rdParty/gmock/gtest/include)
    include_directories(${CMAKE_SOURCE_DIR}/3rdParty/gmock/include)
endif()

# global vars
find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})
message(STATUS ${Boost_INCLUDE_DIRS})

add_subdirectory(src)
if(NOT DISABLE_TESTS)
    add_subdirectory(tests)
endif()
