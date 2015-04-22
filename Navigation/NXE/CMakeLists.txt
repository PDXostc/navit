# cmake preparation
project(NXE CXX)
cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

include_directories(${CMAKE_SOURCE_DIR}/3rdParty/boost-process)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fruit/include)

if (NOT NXE_TARGET_OS)
    set (NXE_TARGET_OS "Linux")
endif()

# add __FILENAME__ macro definition
# however this doesn't work for Ninja, so we need to
# use GCC __BASE_FILE__ macro
if(CMAKE_GENERATOR STREQUAL "Ninja")
    message(STATUS "Ninja")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__=__BASE_FILE__")
else()
    message(STATUS "Not ninja")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
endif()

add_definitions(-std=c++11)
add_definitions(-DBOOST_SPIRIT_THREADSAFE)
enable_testing()

if(NOT DISABLE_TESTS)
    if(NOT NAVIT_PATH)
        message(FATAL_ERROR "Unable to properly do unit testing without navit exectuable")
    else()
        message(STATUS "Navit path set to" ${NAVIT_PATH})
    endif()
    add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/gmock)
    set (BENCHMARK_ENABLE_TESTING OFF)
    add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/benchmark)
endif()

set(boost_components system filesystem thread chrono iostreams timer)

# find Boost
find_package(Boost 1.57.0 EXACT REQUIRED COMPONENTS ${boost_components})
include_directories(${Boost_INCLUDE_DIRS} ${DBUS_INCLUDE_DIRS})

add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/fruit)
add_subdirectory(src)
if(NOT DISABLE_TESTS)
    add_subdirectory(tests)
endif()

if("${NXE_TARGET_OS}" STREQUAL "Tizen")
    add_subdirectory(tests/tizen-tests)
endif()

install (FILES nxe.conf
         DESTINATION share/nxe
 )
install (FILES org.nxe.navit.desktop
         DESTINATION share/applications
 )
