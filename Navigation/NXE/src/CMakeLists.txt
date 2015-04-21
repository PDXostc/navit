include_directories(.)

string(TOUPPER ${NXE_SPEECH} nxe_speech_string)
set( speech_definition "-DNXE_SPEECH_${nxe_speech_string}")
add_definitions(${speech_definition})

add_subdirectory(nxe)
add_subdirectory(mapdownloader)
add_subdirectory(app)

