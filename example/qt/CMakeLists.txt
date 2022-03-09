add_executable(
    microtex-qt-example
    qt_texwidget.cpp
    qt_mainwindow.cpp
    qt_main.cpp
    ../samples/samples.cpp
)

target_include_directories(microtex-qt-example PUBLIC ../samples)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets REQUIRED)

target_link_libraries(
    microtex-qt-example PRIVATE
    microtex
    microtex-qt
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(microtex-qt-example PROPERTIES AUTOMOC ON)
