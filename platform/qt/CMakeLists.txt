message(STATUS "Cross-building using Qt")

add_library(clatexmath-qt SHARED graphic_qt.cpp)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets REQUIRED)

target_include_directories(clatexmath-qt PUBLIC .)

target_link_libraries(
    clatexmath-qt PRIVATE
    clatexmath
    Qt${QT_VERSION_MAJOR}::Gui
)
