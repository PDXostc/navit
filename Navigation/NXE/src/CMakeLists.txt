
set( Python_ADDITIONAL_VERSIONS 2.7 )
set(Python_FIND_VERSION 2)
set(Python_FIND_VERSION_MAJOR 2)
set(API_CONSTANT_NAME kAscii_nxe_api)

# Find Python interpreter
find_package(PythonInterp REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/tec)
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/src/generated_api.cc
                   COMMAND ${PYTHON_EXECUTABLE}
                   ARGS ${CMAKE_SOURCE_DIR}/tec/tools/generate_api.py
                        ${CMAKE_SOURCE_DIR}/js/nxe_api.js
                        ${API_CONSTANT_NAME}
                        generated_api.cc
)

# find DBus
find_package(DBus REQUIRED)
find_package(DBusCpp REQUIRED)
include_directories(${DBUS_INCLUDE_DIR} ${DBUS_ARCH_INCLUDE_DIR} ${DBUSCPP_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set (dbus_library navitdbus.cc)
set(NXE_SRCS
    nxe_extension.cc
    nxe_instance.cc
    navitprocess.h
    navitipc.h
    navitprocessimpl.cc
    navitcontroller.cc
    calls.h
    log.h
    ${dbus_library}
    jsonmessage.cc
    settings.cc
    settingtags.h

    # tizen-extension-crosswalk sources
    ../tec/common/extension.cc

    # generated sources
    ${CMAKE_BINARY_DIR}/src/generated_api.cc
)

add_library(nxe SHARED ${NXE_SRCS})
target_link_libraries(nxe ${Boost_LIBRARIES} ${DBUS_LIBRARIES} ${DBUSCPP_LIBRARIES} )

install(TARGETS nxe
        DESTINATION lib/tizen-extensions-crosswalk)
