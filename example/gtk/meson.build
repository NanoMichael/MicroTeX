example_gtk_src = [
    'gtkmm_main.cpp',
    '../samples/samples.cpp'
]
example_gtk_inc = '../samples'

example_gtk_deps = [
    dependency('gtkmm-3.0'),
    dependency('gtksourceviewmm-3.0'),
    microtex_cairo_dep
]

executable(
    'microtex-gtk-example', example_gtk_src,
    include_directories: example_gtk_inc,
    link_with: [microtex_cairo_lib, microtex_lib],
    dependencies: example_gtk_deps,
    install: true
)
