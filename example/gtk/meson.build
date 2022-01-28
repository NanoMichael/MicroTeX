example_gtk_src = [
    'gtkmm_main.cpp',
    '../samples/samples.cpp'
]
example_gtk_inc = '../samples'

example_gtk_deps = [
    dependency('gtkmm-3.0'),
    dependency('gtksourceviewmm-3.0'),
    clatexmath_cairomm_dep
]

executable(
    'LaTeXGtkExample', example_gtk_src,
    include_directories: example_gtk_inc,
    link_with: [],
    dependencies: example_gtk_deps,
    install: true
)
