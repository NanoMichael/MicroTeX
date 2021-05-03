cmake_minimum_required(VERSION 3.16)
project(LaTeX)

# MSVC Compat

if (MSVC)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    add_compile_options("/utf-8")
else()

# check if compiler has c++11/c++17 support

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)

# check gcc version

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 9)
        # needs extra lib to use std::filesystem
    	set(CXX_FILESYSTEM_LIBRARIES "stdc++fs")
	endif()
    if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 8)
        # dose not have full c++17 features
		set(COMPILER_SUPPORTS_CXX17 OFF)
    endif()
endif()

if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no c++11 support. Please use a different one that supports c++11.")
endif()
endif()
# copy res dir

file(COPY res DESTINATION .)

# source files

file(GLOB_RECURSE SRC "src/*.cpp")

# check operating system

if(QT)
    message(STATUS, "Cross platform build using Qt")
    add_compile_definitions(BUILD_QT)
    include_directories("src")
    set(CMAKE_AUTOMOC ON)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Quick REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Quick REQUIRED)
    set(LINK_LIBS Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Quick)
elseif(SKIA)
    message(STATUS, "Cross platform build using Qt and Skia for rendering")
    add_compile_definitions(BUILD_SKIA)
    add_compile_definitions(SK_GL)
    include_directories(src ../skia ../skia/include)
    link_directories(../skia)
    set(CMAKE_AUTOMOC ON)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(LINK_LIBS Qt5::Widgets skia)
elseif(WIN32)
    message(STATUS "We are working on Windows")
    add_compile_definitions(BUILD_WIN32)
    include_directories("src")
    set(LINK_LIBS gdiplus)
elseif(UNIX)
    message(STATUS "We are working with GTK on a Unix like OS")
    add_compile_definitions(BUILD_GTK)
    find_package(PkgConfig)
    find_package(Fontconfig REQUIRED)
    # include gtkmm module
    pkg_check_modules(GTKMM gtkmm-3.0)
    # include gtksourceview
    pkg_check_modules(GSVMM gtksourceviewmm-3.0)
    # includes and libraries
    link_directories(${GTKMM_LIBRARY_DIRS} ${GSVMM_LIBRARY_DIRS} ${Fontconfig_LIBRARY_DIRS})
    include_directories(${GTKMM_INCLUDE_DIRS} ${GSVMM_INCLUDE_DIRS} ${Fontconfig_INCLUDE_DIRS} "src")
    set(LINK_LIBS ${GTKMM_LIBRARIES} ${GSVMM_LIBRARIES} ${Fontconfig_LIBRARIES})
else()
    message(STATUS "We are working on a unknown platform")
    # other platforms...
endif()

# compile options

option(HAVE_LOG "If enable log" ON)
if(HAVE_LOG)
    add_definitions(-DHAVE_LOG)
endif()

option(GRAPHICS_DEBUG "If enable graphics debug" ON)
if(GRAPHICS_DEBUG)
    add_definitions(-DGRAPHICS_DEBUG)
endif()

option(MEM_CHECK "If compile for memory check only" OFF)
if(MEM_CHECK)
    add_definitions(-DMEM_CHECK)
endif()

option(QT "Compile using Qt instead of Win32/Gtk" OFF)

add_executable(LaTeX ${SRC})
target_link_libraries(LaTeX PUBLIC ${LINK_LIBS} ${CXX_FILESYSTEM_LIBRARIES})
