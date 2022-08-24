cmake_minimum_required(VERSION 3.10)

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

message(STATUS "Building for flutter")

# preset compile variables
if (DEFINED IS_OSX_IOS AND IS_OSX_IOS)
    set(_BUILD_STATIC TRUE)
endif ()
set(_DISABLE_ALIAS TRUE)
set(_HAVE_AUTO_FONT_FIND FALSE)
set(_GLYPH_RENDER_TYPE 0)
set(HAVE_CWRAPPER TRUE)

# win32 has different symbolic-link type
if (WIN32)
    add_subdirectory(wlib)
else ()
    add_subdirectory(lib)
endif ()

if (DEFINED IS_OSX_IOS AND IS_OSX_IOS)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/products)
    install(TARGETS microtex DESTINATION lib)
endif ()
