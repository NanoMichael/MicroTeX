add_executable(
    LaTeXQtSample
    qt_texwidget.cpp
    qt_mainwindow.cpp
    qt_main.cpp
    ../samples/samples.cpp
)

target_include_directories(LaTeXQtSample PUBLIC ../samples)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets REQUIRED)

target_link_libraries(
    LaTeXQtSample PRIVATE
    clatexmath
    clatexmath-qt
    Qt${QT_VERSION_MAJOR}::Widgets
)

set_target_properties(LaTeXQtSample PROPERTIES AUTOMOC ON)
