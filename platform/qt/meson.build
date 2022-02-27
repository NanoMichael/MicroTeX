qt5 = import('qt5')
qt_dep = dependency('qt5', modules: ['Gui'])
qt_src = [
  qt5.preprocess(moc_headers: 'graphic_qt.h', include_directories: inc, dependencies: qt_dep),
  'graphic_qt.cpp'
]

tinytex_qt_lib = library('tinytex-qt', qt_src,
    link_with: tinytex_lib,
    include_directories: inc,
    dependencies: qt_dep,
    version: meson.project_version(),
    soversion: tinytex_api_version,
    install: true
)

tinytex_qt_dep = declare_dependency(
	link_with: [tinytex_lib, tinytex_qt_lib],
	include_directories: ['.', inc],
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(tinytex_qt_lib,
        libraries: tinytex_lib,
        version: meson.project_version(),
        name: 'tinytex-qt',
        filebase: 'tinytex-qt',
        subdirs: ['tinytex/platform/qt'],
        description: 'qt5 backend to tinytex'
    )
endif

if install_headerfiles
    install_headers([
        'graphic_qt.h'
    ], subdir: 'tinytex/platform/qt')
endif
