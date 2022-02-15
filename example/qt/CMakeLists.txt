add_executable(
    QtExample
    qt_texwidget.cpp
    qt_mainwindow.cpp
    qt_main.cpp
    ../samples/samples.cpp
)

target_include_directories(QtExample PUBLIC ../samples)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets REQUIRED)

target_link_libraries(
    QtExample PRIVATE
    tinytex
    tinytex-qt
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(QtExample PROPERTIES AUTOMOC ON)
