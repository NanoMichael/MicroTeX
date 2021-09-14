cmake_minimum_required(VERSION 3.16)
project(LaTeX)

add_library(LaTeX "")

# MSVC Compat

if (MSVC)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    add_compile_options("/utf-8")
    target_compile_features(LaTeX PUBLIC cxx_std_17)
else ()

    # check if compiler has c++17 support

    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

    if (COMPILER_SUPPORTS_CXX17)
        message(STATUS "${CMAKE_CXX_COMPILER} has c++17 support.")
        target_compile_features(LaTeX PUBLIC cxx_std_17)
    else ()
        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no c++17 support. Please use a different one that supports c++17.")
    endif ()
endif ()

# copy res dir

file(COPY res DESTINATION .)

# source files
target_sources(
    LaTeX PRIVATE
    # atom folder
    src/atom/atom_basic.cpp
    src/atom/atom_char.cpp
    src/atom/atom_misc.cpp
    src/atom/atom_matrix.cpp
    src/atom/atom_row.cpp
    src/atom/atom_space.cpp
    src/atom/atom_stack.cpp
    src/atom/atom_accent.cpp
    src/atom/atom_scripts.cpp
    src/atom/atom_vrow.cpp
    src/atom/atom_operator.cpp
    src/atom/atom_zstack.cpp
    src/atom/atom_sideset.cpp
    src/atom/atom_font.cpp
    src/atom/atom_delim.cpp
    src/atom/atom_root.cpp
    src/atom/atom_frac.cpp
    src/atom/atom_fence.cpp
    src/atom/atom_box.cpp
    src/atom/atom_text.cpp
    src/atom/colors_def.cpp
    # box folder
    src/box/box.cpp
    src/box/box_factory.cpp
    src/box/box_group.cpp
    src/box/box_single.cpp
    # core folder
    src/core/split.cpp
    src/core/formula.cpp
    src/core/formula_def.cpp
    src/core/glue.cpp
    src/core/localized_num.cpp
    src/core/parser.cpp
    src/core/debug_config.cpp
    # macro folder
    src/macro/macro.cpp
    src/macro/macro_def.cpp
    src/macro/macro_misc.cpp
    src/macro/macro_scripts.cpp
    src/macro/macro_accent.cpp
    src/macro/macro_colors.cpp
    src/macro/macro_space.cpp
    src/macro/macro_delims.cpp
    src/macro/macro_frac.cpp
    src/macro/macro_styles.cpp
    src/macro/macro_fonts.cpp
    # env folder
    src/env/env.cpp
    src/env/units.cpp
    # utils folder
    src/utils/string_utils.cpp
    src/utils/utf.cpp
    src/utils/utils.cpp
    # otf folder
    src/otf/clm.cpp
    src/otf/glyph.cpp
    src/otf/otf.cpp
    # unimath folder
    src/unimath/math_type.cpp
    src/unimath/uni_char.cpp
    src/unimath/uni_font.cpp
    src/unimath/uni_symbol.cpp
    # graphic folder
    src/graphic/font_style.cpp

    src/latex.cpp
    src/render.cpp
)
target_include_directories(LaTeX PUBLIC src)

# check operating system

if (QT)
    message(STATUS "Cross platform build using Qt")
    target_compile_definitions(LaTeX PUBLIC -DBUILD_QT)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets REQUIRED)
    target_sources(
        LaTeX PRIVATE
        src/platform/qt/graphic_qt.cpp
    )
    target_link_libraries(
        LaTeX PRIVATE
        Qt${QT_VERSION_MAJOR}::Gui
    )
    add_executable(
        LaTeXQtSample
        src/samples/qt_texwidget.cpp
        src/samples/qt_mainwindow.cpp
        src/samples/qt_main.cpp
    )
    target_link_libraries(
        LaTeXQtSample PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets LaTeX
    )
    set_target_properties(LaTeXQtSample PROPERTIES OUTPUT_NAME LaTeX)
    set_target_properties(LaTeXQtSample PROPERTIES AUTOMOC ON)
elseif (SKIA)
    message(STATUS, "Cross platform build using Qt and Skia for rendering")
    target_compile_definitions(LaTeX PUBLIC -DBUILD_SKIA -DSK_GL)
    target_include_directories(LaTeX PUBLIC src)
    if (MSVC)
        find_package(skia REQUIRED)
        target_link_libraries(LaTeX INTERFACE skia skia::skia)
    else ()
        include_directories(../skia ../skia/include)
        link_directories(../skia)
    endif ()
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets REQUIRED)
    target_sources(
        LaTeX PRIVATE
        src/platform/skia/graphic_skia.cpp
    )
    target_link_libraries(
        LaTeX PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
    )
    add_executable(
        LaTeXQtSkiaSample
        src/samples/qt_skiatexwidget.cpp
        src/samples/qt_mainwindow.cpp
        src/samples/qt_main.cpp
    )
    target_link_libraries(
        LaTeXQtSkiaSample PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets LaTeX
    )
    set_target_properties(LaTeXQtSkiaSample PROPERTIES OUTPUT_NAME LaTeX)
    set_target_properties(LaTeXQtSkiaSample PROPERTIES AUTOMOC ON)
elseif (WIN32)
    message(STATUS "We are working on Windows")
    target_compile_definitions(LaTeX PUBLIC -DBUILD_WIN32 -D_HAS_STD_BYTE=0)
    add_executable(
        LaTeXWin32Sample WIN32
        src/platform/gdi_win/graphic_win32.cpp
        src/samples/win32_main.cpp
    )
    target_link_libraries(LaTeXWin32Sample PRIVATE gdiplus LaTeX)
    set_target_properties(LaTeXWin32Sample PROPERTIES OUTPUT_NAME LaTeX)
elseif (UNIX)
    message(STATUS "We are working with GTK on a Unix like OS")
    target_compile_definitions(LaTeX PUBLIC -DBUILD_GTK)
    find_package(Fontconfig REQUIRED)
    pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)
    pkg_check_modules(GSVMM REQUIRED IMPORTED_TARGET gtksourceviewmm-3.0)
    pkg_check_modules(CairoMM REQUIRED IMPORTED_TARGET cairomm-1.0)
    target_sources(
        LaTeX PRIVATE
        src/platform/cairo/graphic_cairo.cpp
    )
    target_link_libraries(
        LaTeX PRIVATE
        PkgConfig::GTKMM #include <pangomm/fontdescription.h>
        PkgConfig::CairoMM #include <cairomm/context.h>
        Fontconfig::Fontconfig
    )
    add_executable(
        LaTeXGtkSample
        src/samples/gtkmm_main.cpp
    )
    target_link_libraries(
        LaTeXGtkSample PRIVATE
        PkgConfig::GSVMM
        LaTeX
    )
    set_target_properties(LaTeXGtkSample PROPERTIES OUTPUT_NAME LaTeX)
else ()
    message(STATUS "We are working on a unknown platform")
    # other platforms...
endif ()

# compile options

option(HAVE_LOG "If enable log" ON)
if (HAVE_LOG)
    add_definitions(-DHAVE_LOG)
endif ()

option(GRAPHICS_DEBUG "If enable graphics debug" ON)
if (GRAPHICS_DEBUG)
    add_definitions(-DGRAPHICS_DEBUG)
endif ()

option(MEM_CHECK "If compile for memory check only" OFF)
if (MEM_CHECK)
    add_definitions(-DMEM_CHECK)
endif ()

option(QT "Compile using Qt instead of Win32/Gtk" OFF)


option(BUILD_EXAMPLE "Build examples" OFF)
if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif ()

