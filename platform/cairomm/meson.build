cairomm_src = [
    'graphic_cairomm.cpp'
]

cairomm_deps = [
    dependency('fontconfig'),
    dependency('cairomm-1.0'),
    dependency('pangomm-1.4')
]

clatexmath_cairomm_lib = library('clatexmath-cairomm', cairomm_src,
    link_with: clatexmath_lib,
    include_directories: inc,
    dependencies: cairomm_deps,
    version: meson.project_version(),
    soversion: 0,
    install: true
)

clatexmath_cairomm_dep = declare_dependency(
	link_with: [clatexmath_lib, clatexmath_cairomm_lib],
	include_directories: ['.', inc],
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(clatexmath_cairomm_lib,
        libraries: clatexmath_lib,
        version: meson.project_version(),
        name: 'clatexmath-cairomm',
        filebase: 'clatexmath-cairomm',
        subdirs: ['clatexmath/platform/cairomm'],
        description: 'cairomm-1.0 backend to cLaTeXMath'
    )
endif

if install_headerfiles
    install_headers([
        'graphic_cairomm.h'
    ], subdir: 'clatexmath/platform/cairomm')
endif
