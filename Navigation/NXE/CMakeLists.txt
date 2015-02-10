# cmake preparation
project(NXE CXX)
cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include_directories(${CMAKE_SOURCE_DIR}/3rdParty/boost-process)

add_definitions(-std=c++11)
add_definitions(-DBOOST_LOG_DYN_LINK)
enable_testing()

# 3rd party
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/boost-dbus/include)

if(NOT DISABLE_TESTS)
    add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/gmock)
    include_directories(${CMAKE_SOURCE_DIR}/3rdParty/gmock/gtest/include)
    include_directories(${CMAKE_SOURCE_DIR}/3rdParty/gmock/include)
endif()

# global vars
find_package(Boost REQUIRED COMPONENTS system filesystem log thread)
find_package(DBus REQUIRED)
include_directories(${Boost_INCLUDE_DIRS} ${DBUS_INCLUDE_DIRS})
message(STATUS ${Boost_INCLUDE_DIRS})

add_subdirectory(src)
if(NOT DISABLE_TESTS)
    add_subdirectory(tests)
endif()
