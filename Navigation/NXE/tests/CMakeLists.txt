include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/tec)


set(MINI_MAIN
    testmain.cc
    testutils.cc
    unit/mocks/navitprocessmock.h
    unit/mocks/navitcontrollermock.h
)
add_definitions(-DNAVIT_PATH=\"${NAVIT_PATH}/\")

add_library(testminimain STATIC ${MINI_MAIN})

add_subdirectory(unit)
add_subdirectory(functional)

