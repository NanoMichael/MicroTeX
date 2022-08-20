message(STATUS "Building µTex GTK Widget")

add_library(microtex-gtk SHARED microtexequation.cpp)

set_target_properties(
    microtex-gtk PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${MICROTEX_API_VERSION}
    CXX_VISIBILITY_PRESET hidden
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Glib REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(Gtk REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(Cairo REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(CairoSvg REQUIRED IMPORTED_TARGET cairo-svg)


target_include_directories(
    microtex-gtk PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/microtex-gtk>
)

target_link_libraries(
    microtex-gtk PRIVATE
    microtex-cairo PRIVATE
    microtex
    PkgConfig::Glib
    PkgConfig::Gtk
    PkgConfig::Cairo
    PkgConfig::CairoSvg
)

microtex_install_target(microtex-gtk)
microtex_install_headers(microtex-gtk HEADERS microtexequation.h)
