message(STATUS "Cross-building using Skia")

add_library(microtex-skia SHARED graphic_skia.cpp)

set_target_properties(
    microtex-skia PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${MICROTEX_API_VERSION}
    CXX_VISIBILITY_PRESET hidden
)

# try find skia via cmake module
find_package(skia QUIET)
if (skia_FOUND)
    target_link_libraries(microtex-skia PRIVATE skia skia::skia)
else ()
    message(WARNING "cmake module Skia not found, try user defined configs")
    message(STATUS "You must has skia built with flag -DSK_SL and export the skparagraph module")
    if (NOT DEFINED SKIA_INCLUDE_DIRECTORY)
        message(FATAL_ERROR "Skia include directory not found, try give it by -DSKIA_INCLUDE_DIRECTORY.")
    endif ()
    message(STATUS "SKIA_INCLUDE_DIRECTORY: ${SKIA_INCLUDE_DIRECTORY}")
    if (NOT DEFINED SKIA_LINK_DIRECTORY)
        message(FATAL_ERROR "Skia link directory not found, try give it by -DSKIA_LINK_DIRECTORY.")
    endif ()
    message(STATUS "SKIA_LINK_DIRECTORY: ${SKIA_LINK_DIRECTORY}")
    target_compile_definitions(microtex-skia PUBLIC -DSK_GL)
    target_include_directories(microtex-skia PUBLIC ${SKIA_INCLUDE_DIRECTORY})
    target_link_directories(microtex-skia PUBLIC ${SKIA_LINK_DIRECTORY})
    target_link_libraries(microtex-skia PUBLIC skparagraph skia)
endif ()

target_include_directories(
    microtex-skia PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/microtex-skia>
)
# we need to export the library on Windows
target_compile_definitions(microtex-skia PRIVATE -DMICROTEX_LIBRARY)
target_link_libraries(
    microtex-skia PRIVATE
    microtex
)

microtex_install_target(microtex-skia)
microtex_install_headers(microtex-skia HEADERS graphic_skia.h)
