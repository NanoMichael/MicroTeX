add_executable(
    tinytex-qt-example
    qt_texwidget.cpp
    qt_mainwindow.cpp
    qt_main.cpp
    ../samples/samples.cpp
)

target_include_directories(tinytex-qt-example PUBLIC ../samples)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets REQUIRED)

target_link_libraries(
    tinytex-qt-example PRIVATE
    tinytex
    tinytex-qt
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(tinytex-qt-example PROPERTIES AUTOMOC ON)
