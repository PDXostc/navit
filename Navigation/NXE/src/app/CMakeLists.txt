
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
find_package(Qt5Compositor REQUIRED)

set( srcs
    app-main.cc
    navitsubcompositor.cc
    navitquickproxy.cc
    appsettings.cc
    mapinfoproxy.cc
    navitmapsproxy.cc
    locationproxy.cc
)

file(GLOB qml_files qml/*.qml qml/*.js qml/nxe-ui/*.qml)

qt5_add_resources(qrcs qml.qrc)

add_executable(nxe-app ${srcs} ${qrcs} ${qml_files})
target_link_libraries(nxe-app Qt5::Core Qt5::Qml Qt5::Quick Qt5::Gui Qt5::Compositor nxe)

file(COPY ${CMAKE_SOURCE_DIR}/nxe.conf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS nxe-app
        DESTINATION bin)
