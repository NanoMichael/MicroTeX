add_executable(
    microtex-win32-example
    ../samples/samples.cpp
    win32_main.cpp
)

target_include_directories(microtex-win32-example PUBLIC ../samples)

set_target_properties(microtex-win32-example PROPERTIES WIN32_EXECUTABLE ON)

target_link_libraries(
    microtex-win32-example PRIVATE
    microtex
    microtex-gdi
)
