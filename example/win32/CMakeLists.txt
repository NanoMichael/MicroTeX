add_executable(
    tinytex-win32-example
    ../samples/samples.cpp
    win32_main.cpp
)

target_include_directories(tinytex-win32-example PUBLIC ../samples)

set_target_properties(tinytex-win32-example PROPERTIES WIN32_EXECUTABLE ON)

target_link_libraries(
    tinytex-win32-example PRIVATE
    tinytex
    tinytex-gdi
)
