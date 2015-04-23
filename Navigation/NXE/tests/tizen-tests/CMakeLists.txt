include_directories(${CMAKE_SOURCE_DIR}/3rdParty/gmock/gtest/include)
include_directories(${CMAKE_SOURCE_DIR}/src/nxe)

add_executable(nxe-tizen-test main.cc)
target_link_libraries(nxe-tizen-test gtest nxe)

install(TARGETS nxe-tizen-test
        DESTINATION bin)
