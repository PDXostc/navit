include_directories(${CMAKE_SOURCE_DIR}/src/nxe)

add_executable(nxe-tizen-test nxe-tizen-test-main.cc)
target_link_libraries(nxe-tizen-test nxe)

install(TARGETS nxe-tizen-test
        DESTINATION bin)
