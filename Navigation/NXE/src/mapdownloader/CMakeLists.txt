

add_custom_command(OUTPUT
                   ${CMAKE_BINARY_DIR}/src/mapdownloader/mapdownloaderadaptor.h
                   ${CMAKE_BINARY_DIR}/src/mapdownloader/mapdownloaderproxy.h
                   PRE_BUILD
                   COMMAND "/usr/bin/dbusxx-xml2cpp"
                   ARGS ${CMAKE_SOURCE_DIR}/src/mapdownloader/mapdownloader.service.xml
                        --proxy=mapdownloaderproxy.h
                        --adaptor=mapdownloaderadaptor.h
                )

find_package(DBus REQUIRED)
find_package(DBusCpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
# find curl
find_package(CURL REQUIRED)
include_directories(${DBUS_INCLUDE_DIR} ${DBUS_ARCH_INCLUDE_DIR} ${DBUSCPP_INCLUDE_DIR} ${BOOST_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR}/src/mapdownloader)

set(mapdownloader_src
    mapdownloader_main.cc
    mapdesc.cc
    mapdownloader.cc
    mapdownloaderdbusserver.cc
    mdlog.h

    # generated
    ${CMAKE_BINARY_DIR}/src/mapdownloader/mapdownloaderproxy.h
    ${CMAKE_BINARY_DIR}/src/mapdownloader/mapdownloaderadaptor.h
)

add_executable(mapdownloader ${mapdownloader_src})
target_link_libraries(mapdownloader ${DBUS_LIBRARIES} ${DBUSCPP_LIBRARIES} ${CURL_LIBRARIES} ${Boost_LIBRARIES})

file(COPY ${CMAKE_SOURCE_DIR}/osm_maps.xml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS mapdownloader
        DESTINATION bin)

install (FILES ${CMAKE_SOURCE_DIR}/osm_maps.xml
         DESTINATION share/nxe
)

install (FILES ${CMAKE_SOURCE_DIR}/org.nxe.mapdownloader.service
         DESTINATION share/dbus-1/services
)
