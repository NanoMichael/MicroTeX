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
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)
pkg_check_modules(CairoMM REQUIRED IMPORTED_TARGET cairomm-1.0)

target_include_directories(tinytex-cairo PUBLIC .)

target_link_libraries(
    tinytex-cairo PRIVATE
    tinytex
    PkgConfig::GTKMM   #include <pangomm/fontdescription.h>
    PkgConfig::CairoMM #include <cairomm/context.h>
    Fontconfig::Fontconfig
)
