include_directories(.)

string(TOUPPER ${NXE_TARGET_OS} nxe_target_os_string)
set( nxe_os_definition "-DNXE_OS_${nxe_target_os_string}")
add_definitions(${nxe_os_definition})

add_subdirectory(nxe)
add_subdirectory(mapdownloader)
add_subdirectory(app)

