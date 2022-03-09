qt5 = import('qt5')
qt_dep = dependency('qt5', modules: ['Gui'])
qt_src = [
  qt5.preprocess(moc_headers: 'graphic_qt.h', include_directories: inc, dependencies: qt_dep),
  'graphic_qt.cpp'
]

microtex_qt_lib = library('microtex-qt', qt_src,
    link_with: microtex_lib,
    include_directories: inc,
    dependencies: qt_dep,
    version: meson.project_version(),
    soversion: microtex_api_version,
    install: true
)

microtex_qt_dep = declare_dependency(
	link_with: [microtex_lib, microtex_qt_lib],
	include_directories: ['.', inc],
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(microtex_qt_lib,
        libraries: microtex_lib,
        version: meson.project_version(),
        name: 'microtex-qt',
        filebase: 'microtex-qt',
        subdirs: ['microtex/platform/qt'],
        description: 'qt5 backend to microtex'
    )
endif

if install_headerfiles
    install_headers([
        'graphic_qt.h'
    ], subdir: 'microtex/platform/qt')
endif
