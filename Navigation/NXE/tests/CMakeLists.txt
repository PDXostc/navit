include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/tec)
include_directories(${CMAKE_SOURCE_DIR}/tests)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/gmock/gtest/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/gmock/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/benchmark/include)


set(MINI_MAIN
    testmain.cc
    testutils.cc
    mocks/navitprocessmock.h
    mocks/navitcontrollermock.h
)
add_definitions(-DNAVIT_PATH=\"${NAVIT_PATH}/\")

add_library(testminimain STATIC ${MINI_MAIN})

add_subdirectory(unit)
add_subdirectory(functional)
add_subdirectory(benchmarks)

