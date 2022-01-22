qt5 = import('qt5')
qt_dep = dependency('qt5', modules: ['Gui'])
qt_src = [
  qt5.preprocess(moc_headers: 'graphic_qt.h', include_directories: inc, dependencies: qt_dep),
  'graphic_qt.cpp'
]

clatexmath_qt_lib = library('clatexmath-qt', qt_src,
    link_with: clatexmath_lib,
    include_directories: inc,
    dependencies: qt_dep,
    version: meson.project_version(),
    soversion: 0,
    install: true
)

clatexmath_qt_dep = declare_dependency(
	link_with: [clatexmath_lib, clatexmath_qt_lib],
	include_directories: ['.', inc],
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(clatexmath_qt_lib,
        libraries: clatexmath_lib,
        version: meson.project_version(),
        name: 'clatexmath-qt',
        filebase: 'clatexmath-qt',
        subdirs: ['clatexmath/platform/qt'],
        description: 'qt5 backend to cLaTeXMath'
    )
endif

if install_headerfiles
    install_headers([
        'graphic_qt.h'
    ], subdir: 'clatexmath/platform/qt')
endif
