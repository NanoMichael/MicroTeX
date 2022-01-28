add_executable(
    LaTeXGtkExample
    ../samples/samples.cpp
    gtkmm_main.cpp
)

target_include_directories(LaTeXGtkExample PUBLIC ../samples)

add_definitions(-DBUILD_EXAMPLE_GTK -DWITH_MM)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSVMM REQUIRED IMPORTED_TARGET gtksourceviewmm-3.0)

target_link_libraries(
    LaTeXGtkExample PRIVATE
    PkgConfig::GSVMM
    clatexmath
    clatexmath-cairo
)
