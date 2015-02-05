
set( Python_ADDITIONAL_VERSIONS 2.7 )
set(Python_FIND_VERSION 2)
set(Python_FIND_VERSION_MAJOR 2)
find_package(PythonInterp)
find_package(Boost COMPONENTS system filesystem)

set(API_CONSTANT_NAME kAscii_nxe_api)

include_directories(${CMAKE_SOURCE_DIR}/tec)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/src/generated_api.cc
                   COMMAND ${PYTHON_EXECUTABLE}
                   ARGS ${CMAKE_SOURCE_DIR}/tec/tools/generate_api.py
                        ${CMAKE_SOURCE_DIR}/js/nxe_api.js
                        ${API_CONSTANT_NAME}
                        generated_api.cc
)

set(NXE_SRCS
    nxe_extension.cc
    nxe_instance.cc
    navitprocess.h
    navitprocessimpl.cc

    # tizen-extension-crosswalk sources
    ../tec/common/extension.cc

    # generated sources
    ${CMAKE_BINARY_DIR}/src/generated_api.cc
)

add_library(nxe SHARED ${NXE_SRCS})
target_link_libraries(nxe ${Boost_LIBRARIES})
