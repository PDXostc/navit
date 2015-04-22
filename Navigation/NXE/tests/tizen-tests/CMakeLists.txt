include_directories(${CMAKE_SOURCE_DIR}/3rdParty/gmock/gtest/include)
include_directories(${CMAKE_SOURCE_DIR}/src/nxe)

add_executable(nxe-tizen-tests main.cc)
target_link_libraries(nxe-tizen-tests gtest nxe)
