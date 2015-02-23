# cmake preparation
project(NXE CXX)
cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include_directories(${CMAKE_SOURCE_DIR}/3rdParty/boost-process)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include)

add_definitions(-std=c++11)
add_definitions(-DBOOST_LOG_DYN_LINK)
enable_testing()

if(NOT DISABLE_TESTS)
    add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/gmock)
    include_directories(${CMAKE_SOURCE_DIR}/3rdParty/gmock/gtest/include)
    include_directories(${CMAKE_SOURCE_DIR}/3rdParty/gmock/include)
endif()

set(boost_components system filesystem thread)

# find Boost
find_package(Boost 1.51.0 EXACT REQUIRED COMPONENTS ${boost_components})
include_directories(${Boost_INCLUDE_DIRS} ${DBUS_INCLUDE_DIRS})

add_subdirectory(src)
if(NOT DISABLE_TESTS)
    add_subdirectory(tests)
endif()

install (FILES nxe.conf
         DESTINATION share/nxe
         )
