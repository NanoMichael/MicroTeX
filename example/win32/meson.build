example_gdi_src = [
    'win32_main.cpp',
    '../samples/samples.cpp'
]
example_gdi_inc = '../samples'

example_gdi_deps = [
    gdi_dep,
    tinytex_gdi_dep
]

executable(
    'tinytex-win32-example', example_gdi_src,
    include_directories: example_gdi_inc,
    link_with: [tinytex_gdi_lib, tinytex_lib],
    dependencies: example_gdi_deps,
    install: true
)
