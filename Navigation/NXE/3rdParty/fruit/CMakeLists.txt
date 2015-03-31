project(injection)
cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/include)

#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# Unsafe, only for debugging/benchmarking.
#add_definitions(-DFRUIT_NO_LOOP_CHECK)


set(INSTALL_LIBRARY_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDE_DIR include/fruit CACHE PATH "Installation directory for headers")

set(FRUIT_VERSION "1.0.2")

add_subdirectory(src)
