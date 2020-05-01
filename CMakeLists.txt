cmake_minimum_required(VERSION 3.17)
project(LaTeX)


# check if compiler has c++11 support

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no c++11 support. Please use a different one that supports c++11.")
endif()

# copy res dir

file(COPY res DESTINATION .)

# source files

file(GLOB_RECURSE SRC "src/*.cpp")


# check operating system

if(WIN32)
    message(STATUS "We are working on Windows")
    include_directories("src")
    set(LINK_LIBS gdiplus)
# elseif(UNIX AND NOT APPLE)
elseif(UNIX)
    message(STATUS "We are working on Unix like os")
    find_package(PkgConfig)
    find_package(Fontconfig REQUIRED)
    # include gtkmm module
    pkg_check_modules(GTKMM gtkmm-3.0)
    # include gtksourceview
    pkg_check_modules(GSVMM gtksourceviewmm-3.0)
    # inlcudes and libraries
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

add_executable(LaTeX ${SRC})
target_link_libraries(LaTeX ${LINK_LIBS})
