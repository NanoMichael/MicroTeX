message(STATUS "Cross-building using emscripten")

add_executable(
    microtex-wasm
    capi.cpp
    graphic_wasm.cpp
)

set_target_properties(
    microtex-wasm PROPERTIES
    CXX_VISIBILITY_PRESET hidden
)

target_link_libraries(
    microtex-wasm PRIVATE
    microtex
)

file(COPY jsapi_impl.js DESTINATION .)

set_target_properties(
    microtex-wasm PROPERTIES
    LINK_FLAGS
    "-fexceptions --js-library jsapi_impl.js -s MODULARIZE=1 -s EXPORT_ES6=1\
    -s WASM=1 -s EXPORTED_RUNTIME_METHODS=allocateUTF8,UTF8ToString -s IMPORTED_MEMORY\
    -s ALLOW_MEMORY_GROWTH=1"
)
