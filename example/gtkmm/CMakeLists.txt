add_executable(
    microtex-gtkmm-example
    ../samples/samples.cpp
    gtkmm_main.cpp
)

target_include_directories(microtex-gtkmm-example PUBLIC ../samples)

add_definitions(-DBUILD_EXAMPLE_GTKMM)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSVMM REQUIRED IMPORTED_TARGET gtksourceviewmm-3.0)

target_link_libraries(
    microtex-gtkmm-example PRIVATE
    PkgConfig::GSVMM
    microtex
    microtex-cairo
)
