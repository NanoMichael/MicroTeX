message(STATUS "Build using GDI+ for Windows")

add_library(microtex-gdi SHARED graphic_win32.cpp)

set_target_properties(
    microtex-gdi PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${MICROTEX_API_VERSION}
    CXX_VISIBILITY_PRESET hidden
)

target_compile_definitions(microtex-gdi PRIVATE -DMICROTEX_LIBRARY)

target_include_directories(microtex-gdi PUBLIC .)

target_link_libraries(
    microtex-gdi PUBLIC
    microtex
    gdiplus
)
