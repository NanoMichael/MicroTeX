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

tinytex_cairo_lib = library('tinytex-cairo', cairo_src,
    link_with: tinytex_lib,
    include_directories: inc,
    dependencies: cairo_deps,
    version: meson.project_version(),
    soversion: 0,
    install: true
)

tinytex_cairo_dep = declare_dependency(
	link_with: [tinytex_lib, tinytex_cairo_lib],
	include_directories: ['.', inc],
	dependencies: [
		dependency('cairo'),
		dependency('pango'),
	],
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(tinytex_cairo_lib,
        libraries: tinytex_lib,
        version: meson.project_version(),
        name: 'tinytex-cairo',
        filebase: 'tinytex-cairo',
        subdirs: ['tinytex/platform/cairo'],
        description: 'cairomm backend to tinytex'
    )
endif

if install_headerfiles
    install_headers([
        'graphic_cairo.h'
    ], subdir: 'tinytex/platform/cairo')
endif
