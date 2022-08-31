message(STATUS "Cross-building using GTK")

add_library(microtex-cairo SHARED graphic_cairo.cpp)

set_target_properties(
    microtex-cairo PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${MICROTEX_API_VERSION}
    CXX_VISIBILITY_PRESET hidden
)

find_package(Fontconfig REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(Cairo REQUIRED IMPORTED_TARGET cairo)
pkg_check_modules(Pango REQUIRED IMPORTED_TARGET pango)
pkg_check_modules(Pangocairo REQUIRED IMPORTED_TARGET pangocairo)

target_include_directories(
    microtex-cairo PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/microtex-cairo>
)

# we need to export the library on Windows
target_compile_definitions(microtex-cairo PRIVATE -DMICROTEX_LIBRARY)
target_link_libraries(
    microtex-cairo PRIVATE
    microtex
    Fontconfig::Fontconfig
    PkgConfig::Cairo
    PkgConfig::Pango
    PkgConfig::Pangocairo
)

microtex_install_target(microtex-cairo)
microtex_install_headers(microtex-cairo HEADERS graphic_cairo.h)
