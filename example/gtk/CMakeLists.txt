find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)
set(GRESOURCE_XML microtex-gtk.gresource.xml)
set(GRESOURCE_C   mt_gtk_res.c)
set(GRESOURCE_H   mt_gtk_res.h)
add_custom_command(
    OUTPUT ${GRESOURCE_C}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${GLIB_COMPILE_RESOURCES}
    ARGS
        ${GRESOURCE_XML}
        --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}
        --c-name=mt_gtk_resource
        --generate
        --target=${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
    VERBATIM
    MAIN_DEPENDENCY ${GRESOURCE_XML}
    DEPENDS
        microtex-gtk.ui
)
add_custom_command(
    OUTPUT ${GRESOURCE_H}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${GLIB_COMPILE_RESOURCES}
    ARGS
        ${GRESOURCE_XML}
        --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}
        --c-name=mt_gtk_resource
        --generate
        --target=${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_H}
    VERBATIM
    MAIN_DEPENDENCY ${GRESOURCE_XML}
    DEPENDS
        microtex-gtk.ui
)
add_custom_target(
    microtex-gtk-resource
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
        ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_H}
)

add_executable(
    microtex-gtk-example
    gtk_main.c
    ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
    isamples.cpp
    ../samples/samples.cpp
)
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/${GRESOURCE_C}
    PROPERTIES GENERATED TRUE
)

add_dependencies(microtex-gtk-example microtex-gtk-resource)
target_include_directories(microtex-gtk-example PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ../samples)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Glib REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(Gtk REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(Gsv REQUIRED IMPORTED_TARGET gtksourceview-5)

target_link_libraries(
    microtex-gtk-example PRIVATE
    PkgConfig::Glib
    PkgConfig::Gtk
    PkgConfig::Gsv
    microtex-gtk
)
