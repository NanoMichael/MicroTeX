cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
    microtex
    VERSION 1.0.0
    DESCRIPTION "MicroTeX for wasm"
)

# the API (and ABI) is compatible when MICROTEX_API_VERSION is the same
set(MICROTEX_API_VERSION 1)

message(STATUS "Building for wasm")

set(_BUILD_STATIC TRUE)
set(_DISABLE_ALIAS TRUE)
set(_HAVE_AUTO_FONT_FIND FALSE)
set(_GLYPH_RENDER_TYPE 1)
set(HAVE_CWRAPPER TRUE)

add_subdirectory(lib)

add_executable(microtex-wasm main.cpp)
target_link_libraries(microtex-wasm PRIVATE microtex)

set_target_properties(
    microtex-wasm PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    LINK_FLAGS
    "-fexceptions -s MODULARIZE=1 -s EXPORT_ES6=1\
    -s WASM=1 -s EXPORTED_RUNTIME_METHODS=allocateUTF8,UTF8ToString,addFunction\
    -s EXPORTED_FUNCTIONS=_malloc,_free\
    -s IMPORTED_MEMORY -s ALLOW_MEMORY_GROWTH=1\
    -s RESERVED_FUNCTION_POINTERS=4"
)
