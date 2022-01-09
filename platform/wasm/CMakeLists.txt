message(STATUS "Cross-building using emscripten")

add_executable(
    clatexmath-wasm
    api.cpp
)

target_link_libraries(
    clatexmath-wasm PRIVATE
    clatexmath
)

set_target_properties(
    clatexmath-wasm PROPERTIES
    LINK_FLAGS "-s WASM=1 -s NO_DISABLE_EXCEPTION_CATCHING"
)
