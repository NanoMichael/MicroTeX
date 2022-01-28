message(STATUS "Cross-building using GTK")

add_library(clatexmath-cairo SHARED graphic_cairomm.cpp)

find_package(Fontconfig REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)
pkg_check_modules(CairoMM REQUIRED IMPORTED_TARGET cairomm-1.0)

target_include_directories(clatexmath-cairo PUBLIC .)

target_link_libraries(
    clatexmath-cairo PRIVATE
    clatexmath
    PkgConfig::GTKMM   #include <pangomm/fontdescription.h>
    PkgConfig::CairoMM #include <cairomm/context.h>
    Fontconfig::Fontconfig
)
