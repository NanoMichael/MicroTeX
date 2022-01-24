cmake_minimum_required(VERSION 3.16)

project(
    LaTeX
    VERSION 1.0.0
    DESCRIPTION "A dynamic, cross-platform, and embeddable LaTeX rendering library"
)

# compile option: if have log
option(HAVE_LOG "If enable log" ON)
if (HAVE_LOG)
    add_definitions(-DHAVE_LOG)
endif ()

add_subdirectory(lib)

file(COPY res DESTINATION .)

if (MSVC)
    # MSVC Compat
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    add_compile_options("/utf-8")
    target_compile_features(clatexmath PUBLIC cxx_std_17)
    target_compile_definitions(clatexmath PRIVATE -DCLATEXMATH_LIBRARY)
    target_compile_definitions(clatexmath PUBLIC -D_HAS_STD_BYTE=0)
    # make binaries together
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else ()
    # check if compiler has c++17 support
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

    if (COMPILER_SUPPORTS_CXX17)
        message(STATUS "${CMAKE_CXX_COMPILER} has c++17 support.")
        target_compile_features(clatexmath PUBLIC cxx_std_17)
    else ()
        message(
            FATAL_ERROR
            "The compiler ${CMAKE_CXX_COMPILER} has no c++17 support. Please use one that supports c++17."
        )
    endif ()
endif ()

if (GTK)
    add_subdirectory(platform/cairo)
endif ()

if (QT)
    add_subdirectory(platform/qt)
endif ()

if (GDI)
    add_subdirectory(platform/gdi_win)
endif ()

if (WASM)
    add_subdirectory(platform/wasm)
endif ()

if (BUILD_EXAMPLE_GTK)
    add_subdirectory(example/gtk)
endif ()

if (BUILD_EXAMPLE_QT)
    add_subdirectory(example/qt)
endif ()

if (BUILD_EXAMPLE_QTPNG)
    add_subdirectory(example/qtpng)
endif ()

if (BUILD_EXAMPLE_MEMCHECK)
    add_subdirectory(example/memcheck)
endif ()

if (BUILD_EXAMPLE_WIN32)
    add_subdirectory(example/win32)
endif ()
