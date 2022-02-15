add_executable(
    GtkExample
    ../samples/samples.cpp
    gtkmm_main.cpp
)

target_include_directories(GtkExample PUBLIC ../samples)

add_definitions(-DBUILD_EXAMPLE_GTK)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSVMM REQUIRED IMPORTED_TARGET gtksourceviewmm-3.0)

target_link_libraries(
    GtkExample PRIVATE
    PkgConfig::GSVMM
    tinytex
    tinytex-cairo
)
