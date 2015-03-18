include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/tec)
include_directories(${CMAKE_SOURCE_DIR}/tests)

set(BENCH_SRCS
    render_benchmark.cc
)


foreach(benchSrc ${BENCH_SRCS})
        get_filename_component(benchName ${benchSrc} NAME_WE)
        add_executable(${benchName} ${benchSrc})
        target_link_libraries(${benchName} nxe benchmark ${Boost_LIBRARIES})
        add_test(NAME ${benchName}Test
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                 COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${benchName})
endforeach(benchSrc)
