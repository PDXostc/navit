
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/tec)

set(TEST_SRCS
    navitprocess_test.cc
)

add_executable(tests ${TEST_SRCS})
target_link_libraries(tests nxe)
