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

tinytex_install_target(tinytex-cairo)
tinytex_install_headers(tinytex-cairo HEADERS graphic_cairo.h)
