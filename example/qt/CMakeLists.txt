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

if (SKIA)
    target_compile_definitions(microtex-qt-example PUBLIC BUILD_SKIA)
    if (QT_VERSION_MAJOR GREATER_EQUAL 6)
        find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
        target_link_libraries(
            microtex-qt-example PRIVATE
            Qt${QT_VERSION_MAJOR}::OpenGLWidgets
        )
    endif ()
    target_sources(microtex-qt-example PRIVATE qt_skiatexwidget.cpp)
    target_link_libraries(
        microtex-qt-example PRIVATE
        microtex-skia
    )
endif ()

set_target_properties(microtex-qt-example PROPERTIES AUTOMOC ON)
