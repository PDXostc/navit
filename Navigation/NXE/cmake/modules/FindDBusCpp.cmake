# Written by Nathan Samson <nathansamson@gmail.com>, 2011
#
# License: Public domain.
#
# Defines
#
#  DBUSCPP_INCLUDE_DIRS
#     which contains the include directory for dbus-c++/dbus.h
#
#  DBUSCPP_LIBRARIES
#     which contains the library directory for libdbus-c++-1

#			
find_path(DBUSCPP_INCLUDE_DIR dbus-c++/dbus.h
                 PATH_SUFFIXES include/dbus-c++-1)




find_library(DBUSCPP_LIBRARIES dbus-c++-1
                 PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DBusCpp DEFAULT_MSG
                                        DBUSCPP_LIBRARIES
                                        DBUSCPP_INCLUDE_DIR)
