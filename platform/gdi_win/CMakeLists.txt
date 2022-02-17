message(STATUS "Build using GDI+ for Windows")

add_library(tinytex-gdi SHARED graphic_win32.cpp)

target_compile_definitions(tinytex-gdi PRIVATE -DTINYTEX_LIBRARY)

target_include_directories(tinytex-gdi PUBLIC .)

target_link_libraries(
    tinytex-gdi PUBLIC
    tinytex
    gdiplus
)
