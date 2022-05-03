function(MICROTEX_INSTALL_HEADERS TARGET)
    cmake_parse_arguments(H " " "PREFIX" "HEADERS" ${ARGN})
    foreach (X IN ITEMS ${H_HEADERS})
        if (H_PREFIX)
            set(_DST "include/${TARGET}/${H_PREFIX}")
        else ()
            set(_DST "include/${TARGET}")
        endif ()
        install(FILES ${X} DESTINATION ${_DST})
    endforeach ()
endfunction()

function(MICROTEX_INSTALL_TARGET TARGET)
    install(
        TARGETS ${TARGET}
        EXPORT ${TARGET}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
    )
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        ${TARGET}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    install(
        EXPORT ${TARGET}Targets
        FILE ${TARGET}Targets.cmake
        NAMESPACE microtex::
        DESTINATION lib/cmake/${TARGET}
    )
    configure_file(${TARGET}Config.cmake.in ${TARGET}Config.cmake @ONLY)
    install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}ConfigVersion.cmake"
        DESTINATION lib/cmake/${TARGET}
    )
endfunction()

set(_HAS_MICROTEX_INSTALL TRUE)
