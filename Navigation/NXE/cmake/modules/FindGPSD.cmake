
INCLUDE(FindPackageHandleStandardArgs)

find_path(GPS_INCLUDE_DIR libgpsmm.h
         PATH_SUFFIXES /usr/include /usr/local/include)

find_library(GPS_LIBRARY gps
             PATH_SUFFIXES /usr/lib /usr/local/lib)

find_package_handle_standard_args(GPS DEFAULT_MSG
                                  GPS_LIBRARY
                                  GPS_INCLUDE_DIR
)
