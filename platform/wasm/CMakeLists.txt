message(STATUS "Cross-building using emscripten")

add_executable(
    tinytex
    capi.cpp
    graphic_wasm.cpp
    cmd.cpp
)

target_link_libraries(
    tinytex PRIVATE
    clatexmath
)

file(COPY jsapi_impl.js DESTINATION .)

set_target_properties(
    tinytex PROPERTIES
    LINK_FLAGS
    "-fexceptions --js-library jsapi_impl.js -s MODULARIZE=1 -s EXPORT_ES6=1\
    -s WASM=1 -s EXPORTED_RUNTIME_METHODS=allocateUTF8 -s IMPORTED_MEMORY\
    -s ALLOW_MEMORY_GROWTH=1"
)
