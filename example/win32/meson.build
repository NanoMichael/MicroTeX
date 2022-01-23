example_gdi_src = [
    'win32_main.cpp',
    '../samples/samples.cpp'
]
example_gdi_inc = '../samples'

example_gdi_deps = [
    gdi_dep,
    clatexmath_gdi_dep
]

executable(
    'LaTeXGdiExample', example_gdi_src,
    include_directories: example_gdi_inc,
    link_with: [clatexmath_gdi_lib, clatexmath_lib],
    dependencies: example_gdi_deps,
    install: true
)
