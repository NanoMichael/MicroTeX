example_gtk_src = [
    'gtkmm_main.cpp',
    '../samples/samples.cpp'
]
example_gtk_inc = '../samples'

example_gtk_deps = [
    dependency('gtkmm-3.0'),
    dependency('gtksourceviewmm-3.0'),
    get_option('EXAMPLE_GTK') == 'cairomm' ? clatexmath_cairomm_dep : clatexmath_cairo_dep
]

executable(
    'LaTeXGtkExample', example_gtk_src,
    include_directories: example_gtk_inc,
    cpp_args: get_option('EXAMPLE_GTK') == 'cairomm' ? '-DWITH_MM' : [],
    link_with: [],
    dependencies: example_gtk_deps,
    install: true
)
