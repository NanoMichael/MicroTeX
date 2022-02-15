message(STATUS "Cross-building using Qt")

add_library(tinytex-qt SHARED graphic_qt.cpp)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets REQUIRED)

target_include_directories(tinytex-qt PUBLIC .)

target_compile_definitions(tinytex-qt PRIVATE -DCLATEXMATH_LIBRARY)
target_link_libraries(
    tinytex-qt PRIVATE
    tinytex
    Qt${QT_VERSION_MAJOR}::Gui
)
