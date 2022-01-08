add_executable(
    LaTeXMemCheck
    ../samples/samples.cpp
    mem_check_main.cpp
)

target_include_directories(LaTeXMemCheck PUBLIC ../samples)

target_link_libraries(
    LaTeXMemCheck PRIVATE
    clatexmath
)
