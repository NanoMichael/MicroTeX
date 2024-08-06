cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
    microtex
    VERSION 1.0.0
    DESCRIPTION "A micro, fast, cross-platform, and embeddable LaTeX rendering library"
)

# the API (and ABI) is compatible when MICROTEX_API_VERSION is the same
set(MICROTEX_API_VERSION 1)

if (MSVC) # MSVC Compat
    add_compile_options("/utf-8")
    # make binaries together
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif ()

####################
#  compile option  #
####################
option(CAIRO "Build with CAIRO" OFF)
option(BUILD_EXAMPLE_GTKMM "Build GTKMM example, only work with -DCAIRO=ON flag" OFF)
option(GTK "Build with GTK" OFF)
option(BUILD_EXAMPLE_GTKMM "Build GTK example, only work with -DCAIRO=ON and -DGTK=ON flag" OFF)
option(QT "Build with Qt" OFF)
option(BUILD_EXAMPLE_QT "Build Qt example, only work with -DQT=ON flag" OFF)
option(BUILD_EXAMPLE_QTPNG "Build Qt png example, only work with -DQT=ON flag" OFF)
option(GDI "Build with GDI" OFF)
option(BUILD_EXAMPLE_WIN32 "Build Win32 GDI example, only work with -DGDI=ON flag" OFF)
option(BUILD_EXAMPLE_MEMCHECK "Build memory check example" OFF)
option(HAVE_LOG "If enable log" ON)
if (HAVE_LOG)
    add_definitions(-DHAVE_LOG)
endif ()
option(BUILD_STATIC "If build for static library" OFF)
if (BUILD_STATIC)
    set(_BUILD_STATIC ON)
endif ()

include(MicroTeXInstall.cmake)

add_subdirectory(lib)
add_subdirectory(doc)

file(COPY res DESTINATION .)

#####################
# optional features #
#####################
if (CAIRO)
    add_subdirectory(platform/cairo)
    if (BUILD_EXAMPLE_GTKMM)
        add_subdirectory(example/gtkmm)
    endif ()
endif ()

if (GTK)
    if (CAIRO)
        add_subdirectory(platform/gtk)
        if (BUILD_EXAMPLE_GTK)
            add_subdirectory(example/gtk)
        endif ()
    else ()
        message(FATAL_ERROR "The GTK Widget requires CAIRO to be enabled")
    endif ()
endif ()

if (QT)
    add_subdirectory(platform/qt)
    if (BUILD_EXAMPLE_QT)
        add_subdirectory(example/qt)
    endif ()
    if (BUILD_EXAMPLE_QTPNG)
        add_subdirectory(example/qtpng)
    endif ()
endif ()

if (SKIA)
    add_subdirectory(platform/skia)
endif ()

if (GDI)
    add_subdirectory(platform/gdi_win)
    if (BUILD_EXAMPLE_WIN32)
        add_subdirectory(example/win32)
    endif ()
endif ()

if (WASM)
    add_subdirectory(platform/wasm)
endif ()

if (BUILD_EXAMPLE_MEMCHECK)
    add_subdirectory(example/memcheck)
endif ()

################
# installation #
################

# install resources
install(DIRECTORY res/ DESTINATION share/microtex)

# install prebuilt scripts
install(FILES prebuilt/otf2clm.py DESTINATION bin)
install(PROGRAMS prebuilt/otf2clm.sh DESTINATION bin RENAME otf2clm)
