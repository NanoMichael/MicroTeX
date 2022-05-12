cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0077 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(
    microtex-flutter
    VERSION 1.0.0
    DESCRIPTION "MicroTeX for flutter"
)

# the API (and ABI) is compatible when MICROTEX_API_VERSION is the same
set(MICROTEX_API_VERSION 1)

if (MSVC)
    add_compile_options("/utf-8")
endif ()

# The microtex library, we build it as static lib
set(BUILD_STATIC TRUE)
set(HAVE_AUTO_FONT_FIND FALSE)
set(_GLYPH_RENDER_TYPE 1)
add_subdirectory(lib)

message(STATUS "Building with flutter")
add_library(
    microtex-flutter SHARED
    graphic_flutter.cpp
    dart_reg.cpp
    capi.cpp
)
set_target_properties(
    microtex-flutter PROPERTIES
    CXX_VISIBILITY_PRESET hidden
)
target_link_libraries(
    microtex-flutter PRIVATE
    microtex
)
