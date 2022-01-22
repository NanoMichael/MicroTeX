cairo_src = [
    'graphic_cairo.cpp'
]

cairo_deps = [
    dependency('fontconfig'),
    dependency('gdkmm-3.0')
]

cairo_inc = include_directories('.')

clatexmath_cairo_lib = library('clatexmath-cairo', cairo_src,
    link_with: clatexmath_lib,
    include_directories: inc,
    dependencies: cairo_deps,
    version: meson.project_version(),
    soversion: 0,
    install: true
)

if install_headerfiles
    install_headers([
        'graphic_cairo.h'
    ], subdir: 'clatexmath/platform/cairo')
endif
