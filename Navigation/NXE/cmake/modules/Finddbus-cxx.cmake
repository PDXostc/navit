# - try to find the dbus-cxx library
#
# Finddbus-cxx.cmake
#
# Cache Variables: (probably not for direct use in CMakeLists.txt)
#  DBUS-CXX_ROOT_DIR
#  DBUS-CXX_LIBRARY
#  DBUS-CXX_INCLUDE_DIR
#
# Non-cache variables you might use in your CMakeLists.txt:
#  DBUS-CXX_FOUND
#
#  DBUS-CXX_LIBRARIES
#  DBUS-CXX_INCLUDE_DIRS
#
# Use this module this way:
#  find_package(dbus-cxx)
#  include_directories(DBUS-CXX_INCLUDE_DIRS)
#  add_executable(myapp ${SOURCES})
#  target_link_libraries(myapp ${DBUS-CXX_LIBRARIES})
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (CMake standard module)
#  FindPkgConfig (CMake standard module)
#
# Dependencies:
#  none
#
# BEGIN_DOT_FILE
#  dbus-cxx [ label = "dbus-cxx" ];
# END_DOT_FILE
#
# Author:
#   Andrew Montag <ajmontag@gmail.com>
#
# License:
#   Boost 1.0 <http://www.boost.org/users/license.html>

set(DBUS-CXX_ROOT_DIR
    "${DBUS-CXX_ROOT_DIR}"
    CACHE
    PATH
    "Prefix directory for dbus-cxx")

######
# deps for this package
find_package(DBus REQUIRED)
include_directories(SYSTEM ${DBUS_INCLUDE_DIRS})
set(LIBS ${LIBS} ${DBUS_LIBRARIES})

find_package(SigC++)
if(SIGC++_FOUND)
    include_directories(SYSTEM ${SigC++_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${SigC++_LIBRARIES})
endif()


# If dbus-cxx isn't required, then neither are its dependencies
if(dbus-cxx_FIND_QUIETLY)
    set(_FIND_FLAGS "QUIET")
else()
    set(_FIND_FLAGS "")
endif()

# Now let's find the dbus-cxx library
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(_dbus-cxx_hint QUIET dbus-cxx)
endif()

find_library(DBUS-CXX_LIBRARY
    NAMES
    dbus-cxx
    HINTS
    ${_dbus-cxx_hint_LIBRARY_DIRS}
    PATHS
    ${DBUS-CXX_ROOT_DIR}
    PATH_SUFFIXES
    lib
    lib32
    lib64
    dbus-cxx/dbus-cxx/.libs/
)

find_path(DBUS-CXX_INCLUDE_DIR
    NAMES
    dbus-cxx/dbus-cxx-config.h
    HINTS
    ${_dbus-cxx_hint_INCLUDE_DIRS}
    PATHS
    ${DBUS-CXX_ROOT_DIR}
    PATH_SUFFIXES
    dbus-cxx/
    dbus-cxx-0.8/
)

# handle the QUIETLY and REQUIRED arguments and set DBUS-CXX_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DBUS-CXX
    DEFAULT_MSG
    DBUS-CXX_LIBRARY
    DBUS-CXX_INCLUDE_DIR)

if(DBUS-CXX_FOUND)
    # Set variables containing libraries and their dependencies
    # Always use the plural form for the variables defined by other find modules:
    # they might have dependencies too!
    set(DBUS-CXX_LIBRARIES    ${DBUS-CXX_LIBRARY})
    set(DBUS-CXX_INCLUDE_DIRS ${DBUS-CXX_INCLUDE_DIR})
    mark_as_advanced(DBUS-CXX_LIBRARY DBUS-CXX_INCLUDE_DIR)
endif()

# End of Finddbus-cxx.cmake
