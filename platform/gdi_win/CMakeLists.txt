message(STATUS "Build using GDI+ for Windows")

add_library(tinytex-gdi SHARED graphic_win32.cpp)

set_target_properties(
    tinytex-gdi PROPERTIES
    VERSION ${VERSION_NAME}
    SOVERSION ${VERSION_NUMBER}
    CXX_VISIBILITY_PRESET hidden
)

target_compile_definitions(tinytex-gdi PRIVATE -DTINYTEX_LIBRARY)

target_include_directories(tinytex-gdi PUBLIC .)

target_link_libraries(
    tinytex-gdi PUBLIC
    tinytex
    gdiplus
)
