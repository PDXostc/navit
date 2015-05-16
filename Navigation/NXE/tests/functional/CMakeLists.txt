include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/tec)
include_directories(${CMAKE_SOURCE_DIR}/tests)

set(TEST_SRCS
    nxeinstance_test.cc
    nxerouting_test.cc
    nxemultithreading_test.cc
)


foreach(testSrc ${TEST_SRCS})
        get_filename_component(testName ${testSrc} NAME_WE)
        add_executable(${testName} ${testSrc})
        target_link_libraries(${testName} testminimain nxe gtest gmock ${Boost_LIBRARIES})
        add_test(NAME ${testName}Test
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                 COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${testName})
endforeach(testSrc)
