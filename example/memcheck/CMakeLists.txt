add_executable(
    MemCheck
    ../samples/samples.cpp
    mem_check_main.cpp
)

target_include_directories(MemCheck PUBLIC ../samples)

target_link_libraries(
    MemCheck PRIVATE
    titnytex
)
