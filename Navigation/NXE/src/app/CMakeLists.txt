
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

include_directories(../nxe)
include_directories(${CMAKE_SOURCE_DIR}/tec)

# Find the QtWidgets library
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)

set( srcs
    main.cc
    navititem.cc
    navitquickproxy.cc
)

qt5_add_resources( qrcs qml.res)

add_executable(nxe-app ${srcs} ${qrcs})
target_link_libraries(nxe-app Qt5::Core Qt5::Qml Qt5::Quick Qt5::Gui nxe)

file(COPY ${CMAKE_SOURCE_DIR}/nxe.conf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS nxe-app
        DESTINATION bin)
