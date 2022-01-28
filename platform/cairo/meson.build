cairo_src = [
    'graphic_cairo.cpp'
]

cairo_deps = [
    dependency('fontconfig'),
    dependency('cairo'),
    dependency('cairo-ft'),
    dependency('pango'),
    dependency('pangocairo')
]

clatexmath_cairo_lib = library('clatexmath-cairo', cairo_src,
    link_with: clatexmath_lib,
    include_directories: inc,
    dependencies: cairo_deps,
    version: meson.project_version(),
    soversion: 0,
    install: true
)

clatexmath_cairo_dep = declare_dependency(
	link_with: [clatexmath_lib, clatexmath_cairo_lib],
	include_directories: ['.', inc],
	dependencies: [
		dependency('cairo'),
		dependency('pango'),
	],
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(clatexmath_cairo_lib,
        libraries: clatexmath_lib,
        version: meson.project_version(),
        name: 'clatexmath-cairo',
        filebase: 'clatexmath-cairo',
        subdirs: ['clatexmath/platform/cairo'],
        description: 'cairomm backend to cLaTeXMath'
    )
endif

if install_headerfiles
    install_headers([
        'graphic_cairo.h'
    ], subdir: 'clatexmath/platform/cairo')
endif
