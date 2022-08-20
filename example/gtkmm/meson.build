example_gtkmm_src = [
    'gtkmm_main.cpp',
    '../samples/samples.cpp'
]
example_gtkmm_inc = '../samples'

example_gtkmm_deps = [
    dependency('gtkmm-3.0'),
    dependency('gtksourceviewmm-3.0'),
    microtex_cairo_dep
]

executable(
    'microtex-gtkmm-example', example_gtkmm_src,
    include_directories: example_gtkmm_inc,
    link_with: [microtex_cairo_lib, microtex_lib],
    dependencies: example_gtkmm_deps,
    install: true
)
