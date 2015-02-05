
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/tec)

set(TEST_SRCS
    navitprocess_test.cc
)

add_executable(navitprocesstest ${TEST_SRCS})
target_link_libraries(navitprocesstest nxe gmock_main)
#GTEST_ADD_TESTS(NavitProcessTest "" ${TEST_SRCS})
