cmake_minimum_required(VERSION 3.16)

project(
    tinytex
    DESCRIPTION "A tiny, fast, cross-platform, and embeddable LaTeX rendering library"
)

include(${PROJECT_SOURCE_DIR}/cmake/version.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (MSVC) # MSVC Compat
    add_compile_options("/utf-8")
    # make binaries together
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else ()
    # check if compiler has c++17 support
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

    if (COMPILER_SUPPORTS_CXX17)
        message(STATUS "${CMAKE_CXX_COMPILER} has c++17 support.")
    else ()
        message(
            FATAL_ERROR
            "The compiler ${CMAKE_CXX_COMPILER} has no c++17 support. Please use one that supports c++17."
        )
    endif ()
endif ()

####################
#  compile option  #
####################
option(GTK "Build with GTK" OFF)
option(BUILD_EXAMPLE_GTK "Build GTK example, only work with -DGTK=ON flag" OFF)
option(QT "Build with Qt" OFF)
option(BUILD_EXAMPLE_QT "Build Qt example, only work with -DQT=ON flag" OFF)
option(BUILD_EXAMPLE_QTPNG "Build Qt png example, only work with -DQT=ON flag" OFF)
option(GDI "Build with GDI" OFF)
option(BUILD_EXAMPLE_WIN32 "Build Win32 GDI example, only work with -DGDI=ON flag" OFF)
option(WASM "Build with WebAssembly" OFF)
option(BUILD_EXAMPLE_MEMCHECK "Build memory check example" OFF)
option(HAVE_LOG "If enable log" ON)
if (HAVE_LOG)
    add_definitions(-DHAVE_LOG)
endif ()

add_subdirectory(lib)

file(COPY res DESTINATION .)

if (GTK)
    add_subdirectory(platform/cairo)
    if (BUILD_EXAMPLE_GTK)
        add_subdirectory(example/gtk)
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
