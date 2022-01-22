example_gtk_src = [
    'gtkmm_main.cpp',
    '../samples/samples.cpp'
]
example_gtk_inc = inc
example_gtk_inc += '../samples'
example_gtk_inc += cairo_inc

example_gtk_deps = [
    dependency('gtkmm-3.0'),
    dependency('gtksourceviewmm-3.0')
]

executable(
    'LaTeXGtkExample', example_gtk_src,
    include_directories: example_gtk_inc,
    link_with: [clatexmath_cairo_lib, clatexmath_lib],
    dependencies: example_gtk_deps,
    install: true
)
