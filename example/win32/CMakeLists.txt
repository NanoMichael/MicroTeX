add_executable(
    Win32Example
    ../samples/samples.cpp
    win32_main.cpp
)

target_include_directories(Win32Example PUBLIC ../samples)

set_target_properties(Win32Example PROPERTIES WIN32_EXECUTABLE ON)

target_link_libraries(
    Win32Example PRIVATE
    tinytex
    tinytex-gdi
)
