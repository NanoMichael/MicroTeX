gdi_src = [
    'graphic_win32.cpp'
]

gdi_deps = [
    gdi_dep
]

clatexmath_gdi_lib = library('clatexmath-gdi', gdi_src,
    link_with: [clatexmath_lib],
    include_directories: inc,
    dependencies: gdi_deps,
    version: meson.project_version(),
    soversion: 0,
    install: true
)

clatexmath_gdi_dep = declare_dependency(
	link_with: [clatexmath_lib, clatexmath_gdi_lib],
	include_directories: ['.', inc],
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(clatexmath_gdi_lib,
        libraries: clatexmath_lib,
        version: meson.project_version(),
        name: 'clatexmath-gdi',
        filebase: 'clatexmath-gdi',
        subdirs: ['clatexmath/platform/gdi_win'],
        description: 'win32 backend to cLaTeXMath'
    )
endif

if install_headerfiles
    install_headers([
        'graphic_win32.h'
    ], subdir: 'clatexmath/platform/gdi_win')
endif
