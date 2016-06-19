configure_file(
        ${PROJECT_SOURCE_DIR}/doc/Doxyfile.in
        ${PROJECT_BINARY_DIR}/doc/Doxyfile
        @ONLY)

configure_file(
        ${PROJECT_SOURCE_DIR}/doc/DoxygenLayout.xml
        ${PROJECT_BINARY_DIR}/doc/DoxygenLayout.xml
        COPYONLY)


if (DOXYGEN_EXECUTABLE)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/)
    if (UNIX)
        add_custom_target(doc ALL
                COMMAND rm -rf html
                COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    elseif (WIN32)
        add_custom_target(doc ALL
                COMMAND if exist html rmdir /s /q html
                COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif (UNIX)
    #INSTALL(
    #  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/
    #  DESTINATION ${INSTALL_DOC_DIR}
    #  COMPONENT html_documentation)
endif (DOXYGEN_EXECUTABLE)

