add_executable(
    tinytex-memcheck
    ../samples/samples.cpp
    mem_check_main.cpp
)

target_include_directories(tinytex-memcheck PUBLIC ../samples)

target_link_libraries(
    tinytex-memcheck PRIVATE
    tinytex
)
