message(STATUS "Build using GDI+ for Windows")

add_library(clatexmath-gdi SHARED graphic_win32.cpp)

target_compile_definitions(clatexmath-gdi PRIVATE -DCLATEXMATH_LIBRARY)

target_include_directories(clatexmath-gdi PUBLIC .)

target_link_libraries(
    clatexmath-gdi PUBLIC
    clatexmath
    gdiplus
)
