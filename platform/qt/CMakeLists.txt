message(STATUS "Cross-building using Qt")

add_library(tinytex-qt SHARED graphic_qt.cpp)

set_target_properties(
    tinytex-qt PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${TINYTEX_API_VERSION}
    CXX_VISIBILITY_PRESET hidden
)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets REQUIRED)

target_include_directories(
    tinytex-qt PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/tinytex-qt>
)

target_compile_definitions(tinytex-qt PRIVATE -DTINYTEX_LIBRARY)
target_link_libraries(
    tinytex-qt PRIVATE
    tinytex
    Qt${QT_VERSION_MAJOR}::Gui
)

tinytex_install_target(tinytex-qt)
tinytex_install_headers(tinytex-qt HEADERS graphic_qt.h)
