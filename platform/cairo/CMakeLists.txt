message(STATUS "Cross-building using GTK")

add_library(tinytex-cairo SHARED graphic_cairo.cpp)

set_target_properties(
    tinytex-cairo PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${TINYTEX_API_VERSION}
    CXX_VISIBILITY_PRESET hidden
)

find_package(Fontconfig REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(Cairo REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(Pango REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(Pangocairo REQUIRED IMPORTED_TARGET pangocairo)

target_include_directories(
    tinytex-cairo PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/tinytex-cairo>
)

target_link_libraries(
    tinytex-cairo PRIVATE
    tinytex
    Fontconfig::Fontconfig
    PkgConfig::Cairo
    PkgConfig::Pango
    PkgConfig::Pangocairo
)

################
# installation #
################
install(
    TARGETS tinytex-cairo
    EXPORT tinytex-cairoTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(
    FILES graphic_cairo.h
    DESTINATION include/tinytex-cairo
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    tinytex-cairoConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(
    EXPORT tinytex-cairoTargets
    FILE tinytex-cairoTargets.cmake
    NAMESPACE tinytex::
    DESTINATION lib/cmake/tinytex-cairo
)
configure_file(tinytex-cairoConfig.cmake.in tinytex-cairoConfig.cmake @ONLY)
install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/tinytex-cairoConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/tinytex-cairoConfigVersion.cmake"
    DESTINATION lib/cmake/tinytex-cairo
)
