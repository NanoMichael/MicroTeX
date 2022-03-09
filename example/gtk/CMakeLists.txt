add_executable(
    microtex-gtk-example
    ../samples/samples.cpp
    gtkmm_main.cpp
)

target_include_directories(microtex-gtk-example PUBLIC ../samples)

add_definitions(-DBUILD_EXAMPLE_GTK)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSVMM REQUIRED IMPORTED_TARGET gtksourceviewmm-3.0)

target_link_libraries(
    microtex-gtk-example PRIVATE
    PkgConfig::GSVMM
    microtex
    microtex-cairo
)
