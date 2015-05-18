# cmake preparation
project(NXE CXX)
cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
add_definitions(-Wall -pedantic)

if(NOT NXE_VERSION)
    execute_process(
      COMMAND git rev-parse --abbrev-ref HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_BRANCH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
      COMMAND git log -1 --format=%h
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_COMMIT_HASH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(NXE_VERSION "${GIT_BRANCH}-${GIT_COMMIT_HASH}")
endif()

message(STATUS "Version=" ${NXE_VERSION})

include_directories(${CMAKE_SOURCE_DIR}/3rdParty/boost-process)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fruit/include)

if (NOT NXE_TARGET_OS)
    set (NXE_TARGET_OS "Linux")
endif()

if("${NXE_TARGET_OS}" STREQUAL "Tizen")
    add_definitions(-DNXE_OS_TIZEN)
else ()
    add_definitions(-DNXE_OS_LINUX)
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

if(NOT DISABLE_TESTS)
    if(NOT NAVIT_PATH)
        message(FATAL_ERROR "Unable to properly do unit testing without navit exectuable")
    else()
        message(STATUS "Navit path set to" ${NAVIT_PATH})
    endif()
    add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/gmock)
    enable_testing()
endif()

set(boost_components system filesystem thread chrono iostreams timer)

# find Boost
find_package(Boost 1.57.0 REQUIRED COMPONENTS ${boost_components})
include_directories(${Boost_INCLUDE_DIRS} ${DBUS_INCLUDE_DIRS})


#add_subdirectory(${CMAKE_SOURCE_DIR}/3rdParty/fruit)
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
install (FILES org.nxe.NavIt.xml
         DESTINATION share/packages
 )
install (FILES org.nxe.NavIt.png
    DESTINATION share/icons/default/small
 )
