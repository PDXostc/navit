include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/tec)
include_directories(${CMAKE_SOURCE_DIR}/tests)

set(BENCH_SRCS
    render_benchmark.cc
    message_handling_benchmark.cc
)

set(bench_main
    benchmark_main.cc
    ../testutils.cc
)

add_library (benchmain STATIC ${bench_main})

foreach(benchSrc ${BENCH_SRCS})
        get_filename_component(benchName ${benchSrc} NAME_WE)
        add_executable(${benchName} ${benchSrc})
        target_link_libraries(${benchName} nxe benchmark ${Boost_LIBRARIES} benchmain gtest gmock)
        add_test(NAME ${benchName}Test
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                 COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${benchName})
endforeach(benchSrc)
