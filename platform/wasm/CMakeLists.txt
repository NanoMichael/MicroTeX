message(STATUS "Cross-building using emscripten")

add_executable(
    clatexmath-wasm
    capi.cpp
    graphic_wasm.cpp
    cmd.cpp
)

target_link_libraries(
    clatexmath-wasm PRIVATE
    clatexmath
)

set_target_properties(
    clatexmath-wasm PROPERTIES
    LINK_FLAGS "-fexceptions -s WASM=1"
)
