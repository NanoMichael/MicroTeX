function(TINYTEX_INSTALL TARGET)
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
        NAMESPACE tinytex::
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
