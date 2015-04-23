
# find DBus
find_package(DBus REQUIRED)
find_package(DBusCpp REQUIRED)
find_package(GPSD REQUIRED)
include_directories(${DBUS_INCLUDE_DIR} ${DBUS_ARCH_INCLUDE_DIR} ${DBUSCPP_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set (dbus_library 
    navitdbus.cc
    mapdownloaderdbus.cc
    dbuscontroller.cc
    speechimpldbus.cc
)

set(nxe_interfaces
    inavitprocess.h
    inavitipc.h
    igpsprovider.h
    imapdownloader.h
    ispeech.h
)

set(NXE_SRCS
    # interfaces
    ${nxe_interfaces}

    nxe_extension.cc
    nxe_instance.cc
    navitcontroller.cc
    calls.h
    log.h
    log.cc
    jsonmessage.cc
    settings.cc
    settingtags.h
    position.h
    context.h

    # dbus interface implementation
    ${dbus_library}
    dbus_helpers.hpp

    # navit posix process controlling implementation
    navitprocessimpl.cc

    # gps providers
    gpsdprovider.cc
)

set(libraries ${Boost_LIBRARIES} ${DBUS_LIBRARIES} ${DBUSCPP_LIBRARIES} -lrt fruit ${GPS_LIBRARY})

add_library(nxe SHARED ${NXE_SRCS})
if ("${NXE_SPEECH_IMPL}" STREQUAL "tizen")
    list(APPEND libraries tts)
endif()

target_link_libraries(nxe ${libraries})

#install(TARGETS nxe
#        DESTINATION lib/tizen-extensions-crosswalk)
install(TARGETS nxe
        DESTINATION lib)
