qt5 = import('qt5')
qt_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'])
qt_src = qt5.preprocess(moc_headers: 'graphic_qt.h', include_directories: inc, dependencies: qt_dep)

clatexmath_qt_lib = library('clatexmath-qt', qt_src,
    link_with: clatexmath_lib,
    include_directories: inc,
    dependencies: qt_dep,
    version: meson.project_version(),
    soversion: 0,
    install: true
)

if install_headerfiles
    install_headers([
        'graphic_qt.h'
    ], subdir: 'clatexmath/platform/qt')
endif
