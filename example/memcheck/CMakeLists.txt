add_executable(
    microtex-memcheck
    ../samples/samples.cpp
    mem_check_main.cpp
)

target_include_directories(microtex-memcheck PUBLIC ../samples)

target_link_libraries(
    microtex-memcheck PRIVATE
    microtex
)
