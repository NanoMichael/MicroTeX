add_executable(
    tinytex-gtk-example
    ../samples/samples.cpp
    gtkmm_main.cpp
)

target_include_directories(tinytex-gtk-example PUBLIC ../samples)

add_definitions(-DBUILD_EXAMPLE_GTK)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSVMM REQUIRED IMPORTED_TARGET gtksourceviewmm-3.0)

target_link_libraries(
    tinytex-gtk-example PRIVATE
    PkgConfig::GSVMM
    tinytex
    tinytex-cairo
)
